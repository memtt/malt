#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 01/2026
#    LICENSE  : CeCILL-C
#    FILE     : dev/build_and_test_on_distributions.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024 - 2025
#    AUTHOR   : Sébastien Valat - 2024 - 2026
############################################################

############################################################
'''
Test building MALT + running unit tests on various distributions via
docker/podman to validate it is still OK (or close to) on all.

```sh
# default
./build_and_test_on_distributions.py

# better to see the name of each test as we don't have tons of them
./build_and_test_on_distributions.py -v

# for debugging in real time, get direct output of commands.
./build_and_test_on_distributions.py -v --capture=no

# you can also call it via pytest command itself
pytest ./build_and_test_on_distributions.py
```
'''

############################################################
# python
import sys
import tempfile
import multiprocessing
from contextlib import contextmanager
from common import jump_in_dir, assert_shell_command, in_container, PodmanContainerHandler, get_malt_source_path
# pytest
import pytest

############################################################
COMMON_CONF_OPTIONS = "--with-cargo=$HOME/.cargo"
############################################################
PYTHON_VERSION="3.13.2"
BUILD_CUSTOM_CORES=8
BUILD_CUSTOM_PYTHON = f"""cd /tmp \\
    && curl --continue-at - -o /var/sources/Python-{PYTHON_VERSION}.tar.xz https://www.python.org/ftp/python/3.13.2/Python-{PYTHON_VERSION}.tar.xz \\
    && tar -xf /var/sources/Python-{PYTHON_VERSION}.tar.xz \\
    && cd Python-{PYTHON_VERSION} \\
    && ./configure --enable-shared --prefix=/opt/malt-python \\
    && make -j{BUILD_CUSTOM_CORES} \\
    && make install \\
    && cd .. \\
    && rm -rfd Python-{PYTHON_VERSION} """
############################################################
RUST_SETUP_LAST="""curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs > /tmp/rustup-install.sh \\
    && sh /tmp/rustup-install.sh -y \\
    && rm -f /tmp/rustup-install.sh \\
    && export PATH=$HOME/.cargo/bin:$PATH \\
    && echo 'export PATH=$HOME/.cargo/bin:$PATH' >> ~/.bashrc \\
    && rustup update stable
"""
UBUNTU_BASIC_CMDS=[
    # for local cache usage
    "rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages \"true\";' > /etc/apt/apt.conf.d/keep-cache",
    # update
    "apt update",
    # upgrade
    "apt upgrade -y",
    # install deps for build system
    "apt install -y make gcc cmake g++ clang ccache",
]
UBUNTU_FULL_CMDS=[
    # to build with everything
    "apt install -y libunwind-dev libelf-dev graphviz nlohmann-json3-dev libcpp-httplib-dev python3-dev googletest libiniparser-dev",
    # to play with sources
    "apt install -y git nodejs npm curl",
    # Last cargo / rust
    RUST_SETUP_LAST,
]
UBUNTU_FULL_CMDS_DEBIAN_11=[
    # to build with everything
    "apt install -y libunwind-dev libelf-dev graphviz nlohmann-json3-dev python3-dev googletest libiniparser-dev",
    # to play with sources
    "apt install -y git nodejs npm curl",
    # Last cargo / rust
    RUST_SETUP_LAST,
]
UBUNTU_FULL_CMDS_UBUNTU_25=[
    # to build with everything
    "apt install -y libunwind-dev libelf-dev graphviz nlohmann-json3-dev libcpp-httplib-dev python3-dev googletest libiniparser-dev",
    # to play with sources
    "apt install -y git nodejs npm curl",
    # Cargo / rust (recent enougth)
    "apt install -y cargo",
]
UBUNTU_INDEV_CMDS=[
]
CENTOS_BASIC_CMDS=[
    # for local cache usage
    "echo 'keepcache=true' >> /etc/dnf/dnf.conf",
    # refresh cache
    "dnf makecache --refresh",
    # update
    "dnf update -y",
    # install minimal
    "dnf install -y cmake gcc-c++ make clang ccache",
]
ROCKY_BASIC_CMDS=[
    # for local cache usage
    "echo 'keepcache=true' >> /etc/dnf/dnf.conf",
    # refresh cache
    "dnf makecache --refresh",
    # update
    "dnf update -y",
    # install minimal
    "dnf install -y cmake gcc-c++ make clang",
]

CENTOS_YUM_BASIC_CMDS=[
    # Due to URL changed
    "sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*",
    "sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*",
    # update
    "yum update -y",
    # install minimal
    "yum install -y cmake gcc-c++ make clang",
]
CENTOS_YMU_FULL_CMDS=[
    # install basics
    "yum install -y graphviz python3-devel",
    # playsing with sources
    "yum install -y wget git nodejs npm curl bzip2 xz",
]
ROCKY_FULL_CMDS=[
    # all deps
    "dnf install -y libunwind-devel elfutils-devel graphviz nlohmann-json-devel cpp-httplib-devel python3-devel gtest iniparser-devel",
    # playsing with sources
    "dnf install -y git nodejs npm curl bzip2 xz",
    # cargo
    "dnf install -y cargo"
]
ROCKY_INDEV_CMDS=[
]
REDHAT_OLD_FULL_CMDS=[
    # install basics
    "dnf install -y graphviz python3-devel",
    # playsing with sources
    "dnf install -y --allowerasing wget git nodejs npm curl bzip2 xz",
    # cargo
    "dnf install -y cargo"
]
CENTOS_FULL_CMDS=[
    # all deps
    "dnf install -y libunwind-devel elfutils-devel graphviz nlohmann-json-devel cpp-httplib-devel python3-devel gtest iniparser-devel",
    # playsing with sources
    "dnf install -y git nodejs npm curl",
    # cargo
    "dnf install -y cargo"
]
CENTOS_INDEV_CMDS=[
]
ARCH_BASIC_CMDS=[
    # update
    "pacman --noconfirm -Syu",
    # install build deps
    "pacman --noconfirm -Sy cmake gcc make clang ccache",
]
ARCH_FULL_CMDS=[
    # full deps
    "pacman --noconfirm -Sy libunwind libelf graphviz nlohmann-json python3 gtest iniparser",
    # play with sources
    "pacman --noconfirm -Sy git nodejs npm curl",
    # cargo
    "pacman --noconfirm -Sy cargo"
]
ARCH_INDEV_CMDS=[
]
GENTOO_BASIC_CMDS=[
    # get package list
    "emerge-webrsync",
    # set OK for bin packages
    "yes | getuto",
    # update
    "emerge --verbose --update --deep --newuse @world -k -g",
    # install build deps
    "emerge -k -g cmake gcc make llvm-core/clang ccache",
]
GENTOO_FULL_CMDS=[
    #"eselect profile set default/linux/amd64/23.0/desktop/plasma && emerge --verbose --update --deep --newuse @world -k -g",
    # Set use flags
    "echo 'media-libs/gd fontconfig truetype' > /etc/portage/package.use/01-malt",
    # install full deps
    "emerge -b -k -g sys-libs/libunwind dev-libs/elfutils media-gfx/graphviz dev-cpp/nlohmann_json dev-cpp/cpp-httplib dev-lang/python dev-cpp/gtest dev-libs/iniparser",
    # play with sources
    "emerge -b -k -g dev-vcs/git net-libs/nodejs net-misc/curl",
    # rust
    "emerge -b -k -g dev-util/cargo-c"
]
GENTOO_INDEV_CMDS=[
]
############################################################
BUILD_PARAMETERS = {
    "distributions": {
        ############ ubuntu:22.04
        "malt/ubuntu-basic:22.04": {
            "base": "ubuntu:22.04",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:22.04": {
            "base": "malt/ubuntu-basic:22.04",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/ubuntu-indev:22.04": {
            "base": "malt/ubuntu-full:22.04",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ ubuntu:24.04
        "malt/ubuntu-basic:24.04": {
            "base": "ubuntu:24.04",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:24.04": {
            "base": "malt/ubuntu-basic:24.04",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/ubuntu-indev:24.04": {
            "base": "malt/ubuntu-full:24.04",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ ubuntu:25.04
        "malt/ubuntu-basic:25.04": {
            "base": "ubuntu:25.04",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:25.04": {
            "base": "malt/ubuntu-basic:25.04",
            "cmds": UBUNTU_FULL_CMDS_UBUNTU_25
        },
        "malt/ubuntu-indev:25.04": {
            "base": "malt/ubuntu-full:25.04",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ ubuntu:25.10
        "malt/ubuntu-basic:25.10": {
            "base": "ubuntu:25.10",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:25.10": {
            "base": "malt/ubuntu-basic:25.10",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/ubuntu-indev:25.10": {
            "base": "malt/ubuntu-full:25.10",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ debian:11
        "malt/debian-basic:11": {
            "base": "debian:11",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:11": {
            "base": "malt/debian-basic:11",
            "cmds": UBUNTU_FULL_CMDS_DEBIAN_11
        },
        "malt/debian-indev:11": {
            "base": "malt/debian-full:11",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ debian:12
        "malt/debian-basic:12": {
            "base": "debian:12",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:12": {
            "base": "malt/debian-basic:12",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/debian-indev:12": {
            "base": "malt/debian-full:12",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ debian:13
        "malt/debian-basic:13": {
            "base": "debian:13",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:13": {
            "base": "malt/debian-basic:13",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/debian-indev:13": {
            "base": "malt/debian-full:13",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ debian:sid
        "malt/debian-basic:sid": {
            "base": "debian:sid",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:sid": {
            "base": "malt/debian-basic:sid",
            "cmds": UBUNTU_FULL_CMDS
        },
        "malt/debian-indev:sid": {
            "base": "malt/debian-full:sid",
            "cmds": UBUNTU_INDEV_CMDS
        },
        ############ fedora:41
        "malt/fedora-basic:40": {
            "base": "fedora:40",
            "cmds": CENTOS_BASIC_CMDS
        },
        "malt/fedora-full:40": {
            "base": "malt/fedora-basic:40",
            "cmds": CENTOS_FULL_CMDS
        },
        "malt/fedora-indev:40": {
            "base": "malt/fedora-full:40",
            "cmds": CENTOS_INDEV_CMDS
        },
        ############ fedora:41
        "malt/fedora-basic:41": {
            "base": "fedora:41",
            "cmds": CENTOS_BASIC_CMDS
        },
        "malt/fedora-full:41": {
            "base": "malt/fedora-basic:41",
            "cmds": CENTOS_FULL_CMDS
        },
        "malt/fedora-indev:41": {
            "base": "malt/fedora-full:41",
            "cmds": CENTOS_INDEV_CMDS
        },
        ############ redhat 8
        "malt/redhat-basic:8": {
            "base": "ubi8",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/redhat-full:8": {
            "base": "malt/redhat-basic:8",
            "cmds": REDHAT_OLD_FULL_CMDS
        },
        "malt/reshat-indev:8": {
            "base": "malt/redhat-full:8",
            "cmds": ROCKY_INDEV_CMDS
        },
        "malt/redhat-basic:9": {
            "base": "ubi9",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/redhat-full:9": {
            "base": "malt/redhat-basic:9",
            "cmds": REDHAT_OLD_FULL_CMDS
        },
        "malt/reshat-indev:9": {
            "base": "malt/redhat-full:9",
            "cmds": ROCKY_INDEV_CMDS
        },
        "malt/redhat-basic:10": {
            "base": "ubi10",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/redhat-full:10": {
            "base": "malt/redhat-basic:10",
            "cmds": REDHAT_OLD_FULL_CMDS
        },
        "malt/reshat-indev:10": {
            "base": "malt/redhat-full:10",
            "cmds": ROCKY_INDEV_CMDS
        },
        ############ rocky:8.9
        "malt/rocky-basic:8.9": {
            "base": "rockylinux:8.9",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/rocky-full:8.9": {
            "base": "malt/rocky-basic:8.9",
            "cmds": ROCKY_FULL_CMDS
        },
        "malt/rocky-indev:8.9": {
            "base": "malt/rocky-full:8.9",
            "cmds": ROCKY_INDEV_CMDS
        },
        ############ rocky:9.3
        "malt/rocky-basic:9.3": {
            "base": "rockylinux:9.3",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/rocky-full:9.3": {
            "base": "malt/rocky-basic:9.3",
            "cmds": ROCKY_FULL_CMDS
        },
        "malt/rocky-indev:9.3": {
            "base": "malt/rocky-full:9.3",
            "cmds": [
                BUILD_CUSTOM_PYTHON
            ]
        },
        ############ centos:9
        "malt/centos-basic:7": {
            "base": "centos:7",
            "cmds": CENTOS_YUM_BASIC_CMDS + [
                "yum install -y wget libopenssl-devel",
                f"cd /tmp && wget https://github.com/Kitware/CMake/releases/download/v3.13.3/cmake-3.13.3.tar.gz && tar -xvf cmake-3.13.3.tar.gz && cd cmake-3.13.3 && ./configure && make -j4 && make install && cd .. && rm -rfvd cmake-3.13.3*"
            ]
        },
        "malt/centos-full:7": {
            "base": "malt/centos-basic:7",
            "cmds": CENTOS_YMU_FULL_CMDS
        },
        "malt/centos-basic:8": {
            "base": "centos:8",
            "cmds": CENTOS_YUM_BASIC_CMDS
        },
        "malt/centos-full:8": {
            "base": "malt/centos-basic:8",
            "cmds": CENTOS_YMU_FULL_CMDS
        },
        "malt/centos-basic:9": {
            "base": "centos:9",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/centos-full:9": {
            "base": "malt/centos-basic:9",
            "cmds": REDHAT_OLD_FULL_CMDS
        },
        "malt/centos-basic:10": {
            "base": "centos:10",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/centos-full:10": {
            "base": "malt/centos-basic:10",
            "cmds": REDHAT_OLD_FULL_CMDS
        },
        ############ archlinux:latest
        "malt/archlinux-basic:latest": {
            "base": "docker.io/library/archlinux:latest",
            "cmds": ARCH_BASIC_CMDS
        },
        "malt/archlinux-full:latest": {
            "base": "malt/archlinux-basic:latest",
            "cmds": ARCH_FULL_CMDS
        },
        "malt/archlinux-indev:latest": {
            "base": "malt/archlinux-full:latest",
            "cmds": ARCH_INDEV_CMDS
        },
        ############ archlinux:latest
        "malt/gentoo-basic:latest": {
            "base": "docker.io/gentoo/stage3:latest",
            "cmds": GENTOO_BASIC_CMDS
        },
        "malt/gentoo-full:latest": {
            "base": "malt/gentoo-basic:latest",
            "cmds": GENTOO_FULL_CMDS
        },
        "malt/gentoo-indev:latest": {
            "base": "malt/gentoo-full:latest",
            "cmds": GENTOO_INDEV_CMDS
        },
    },
    'compilers': {
        "gcc": "CXX=g++ CC=gcc",
        #"clang": "CXX=clang++ CC=clang",
    },
    "variants": {
        "debug": "--enable-debug",
        "release": ""
    }
}

############################################################
# for package systems
CACHES = {
    "podman-cache/ccache/@IMG@": "/root/.ccache",
    "podman-cache/apt/@IMG@": "/var/cache/apt",
    "podman-cache/pacman/@IMG@": "/var/cache/pacman/pkg",
    "podman-cache/emerge/@IMG@": "/var/cache/binpkgs",
    "podman-cache/libdnf5/@IMG@": "/var/cache/libdnf5",
    "podman-cache/dnf/@IMG@": "/var/cache/dnf",
    "podman-cache/sources": "/var/sources"
}

############################################################

def get_make_jobs() -> int:
    cores = int(multiprocessing.cpu_count())
    return cores

############################################################
def gen_distr_paramatrized():
    params = []
    for variant in BUILD_PARAMETERS['variants']:
        for compiler in BUILD_PARAMETERS['compilers']:
            for distr_name in BUILD_PARAMETERS['distributions']:
                params.append((distr_name, compiler, variant))
    return params

############################################################
@pytest.mark.parametrize("dist_name_version", BUILD_PARAMETERS['distributions'].keys())
def test_prep_image(dist_name_version):
    # to install
    distr_install_cmds = BUILD_PARAMETERS['distributions'][dist_name_version]["cmds"]
    base_name = BUILD_PARAMETERS['distributions'][dist_name_version]["base"]

    # build
    container = PodmanContainerHandler(dist_name_version)
    container.add_build_run_rules(distr_install_cmds)
    container.build(base_name, caches=CACHES)

############################################################
@pytest.mark.parametrize("dist_name_version, compiler, variant", gen_distr_paramatrized())
def test_distribution(dist_name_version: str, compiler:str, variant:str):
    # extract options
    compiler_options = BUILD_PARAMETERS['compilers'][compiler]
    variant_options = BUILD_PARAMETERS['variants'][variant]

    # infos
    cores = get_make_jobs()

    # options
    dev_options = ""
    if "-indev" in dist_name_version:
        dev_options = "--enable-python"

    # build & start container to run commands in
    with in_container(dist_name_version, caches=CACHES) as container:
        # to perform tests
        container.assert_run(f"/mnt/malt-sources/configure --enable-tests {COMMON_CONF_OPTIONS} {variant_options} {compiler_options} {dev_options}")
        container.assert_run(f"make -j{cores}")
        container.assert_run(f"OMP_NUM_THREADS=1 ctest --output-on-failure -j{cores}")

############################################################
def test_current_host_debug_no_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = get_make_jobs()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-debug CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command(f"make -j{cores}")

############################################################
def test_current_host_debug_disable_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = get_make_jobs()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-debug --disable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command(f"make -j{cores}")

############################################################
def test_current_host_debug_tests():
    # get malt source path
    sources = get_malt_source_path()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-debug --enable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command("make -j8")
            assert_shell_command("ctest --output-on-failure")

############################################################
def test_current_host_release_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = get_make_jobs()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command(f"make -j{cores}")
            assert_shell_command("ctest --output-on-failure")

############################################################
def test_current_host_debug_pedantic_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = get_make_jobs()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-debug --enable-tests CFLAGS='-Wpedantic -Werror' CXXFLAGS='-Wpedantic -Werror'")
            assert_shell_command(f"make -j{cores}")
            assert_shell_command("ctest --output-on-failure")

############################################################
# To be able to run as a standard program directly and not call via pytest command
# (can still also)
if __name__ == '__main__':
    if "-v" in sys.argv:
        sys.argv.append("--capture=no")

    # nicer to get -v by default to get the list of tests displayed as 'integration tests' can be long
    sys.argv.append('-v')

    # we are not testing pythong thing itself, so no need to get the header
    sys.argv.append('--no-header')

    #results
    sys.argv.append('--junitxml=build-portability.junit.xml')

    # self test oursef
    pytest.main(sys.argv)

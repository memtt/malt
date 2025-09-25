#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/build_and_test_on_distributions.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024 - 2025
#    AUTHOR   : Sébastien Valat - 2024
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
BUILD_CUSTOM_CORES=4
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
UBUNTU_BASIC_CMDS=[
    "rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages \"true\";' > /etc/apt/apt.conf.d/keep-cache",
    "apt update",
    "apt upgrade -y",
    "apt install -y cmake g++ make clang ccache",
    "apt install -y libssl-dev"
]
UBUNTU_FULL_CMDS=[
    "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
    "apt install -y curl",
    "apt update && apt install -y nlohmann-json3-dev cargo graphviz",
    """curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs > /tmp/rustup-install.sh \\
    && sh /tmp/rustup-install.sh -y \\
    && rm -f /tmp/rustup-install.sh \\
    && export PATH=$HOME/.cargo/bin:$PATH \\
    && echo 'export PATH=$HOME/.cargo/bin:$PATH' >> ~/.bashrc \\
    && rustup update stable
    """
]
UBUNTU_FULL_CMDS_UBUNTU_25=[
    "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
    "apt install -y curl",
    "apt update && apt install -y nlohmann-json3-dev cargo graphviz"
]
UBUNTU_INDEV_CMDS=[
    "apt update && apt install -y python3-dev"
]
UBUNTU_INDEV_CMDS_UBUNTU_25=[
    "apt update && apt install -y libpython3-dev"
]
CENTOS_BASIC_CMDS=[
    "echo 'keepcache=true' >> /etc/dnf/dnf.conf",
    "dnf makecache --refresh",
    "dnf update -y",
    "dnf install -y cmake gcc-c++ make clang ccache"
]
ROCKY_BASIC_CMDS=[
    "echo 'keepcache=true' >> /etc/dnf/dnf.conf",
    "dnf makecache --refresh",
    "dnf update -y",
    "dnf install -y cmake gcc-c++ make clang",
    "dnf install -y openssl-devel"
]
ROCKY_FULL_CMDS=[
    "dnf install -y elfutils-libelf-devel nodejs npm",
    "dnf install -y curl bzip2 xz",
]
ROCKY_INDEV_CMDS=[
    "dnf install -y python3-devel"
]
CENTOS_FULL_CMDS=[
    "dnf install -y libunwind-devel elfutils-libelf-devel libunwind-devel nodejs npm",
    "dnf install -y curl graphviz",
]
CENTOS_INDEV_CMDS=[
    "dnf install -y python3-devel",
]
ARCH_BASIC_CMDS=[
    "pacman --noconfirm -Syu",
    "pacman --noconfirm -Sy cmake gcc make clang ccache",
    "pacman --noconfirm -Sy openssl"
]
ARCH_FULL_CMDS=[
    "pacman --noconfirm -Sy libunwind libelf nodejs npm",
    "pacman --noconfirm -Sy curl",
    "pacman --noconfirm -Sy nlohmann-json cargo rustup graphviz"
]
ARCH_INDEV_CMDS=[
    "pacman --noconfirm -Sy python3"
]
GENTOO_BASIC_CMDS=[
    "emerge-webrsync",
    "yes | getuto",
    "emerge --verbose --update --deep --newuse @world -k -g",
    "emerge -k -g cmake gcc make llvm-core/clang ccache",
    "emerge -k -g openssl",
]
GENTOO_FULL_CMDS=[
    #"eselect profile set default/linux/amd64/23.0/desktop/plasma && emerge --verbose --update --deep --newuse @world -k -g",
    "emerge -b -k -g sys-libs/libunwind elfutils nodejs",
    #"emerge -k -g dev-qt/qtwebengine",
    "emerge -b -k -g curl",
    "emerge -b -k -g dev-cpp/nlohmann_json dev-util/cargo-c media-gfx/graphviz"
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
            "cmds": UBUNTU_INDEV_CMDS_UBUNTU_25
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
            "cmds": UBUNTU_FULL_CMDS
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
        ############ rocky:8.9
        "malt/rocky-basic:8.9": {
            "base": "rockylinux:8.9",
            "cmds": ROCKY_BASIC_CMDS
        },
        "malt/rocky-full:8.9": {
            "base": "malt/rocky-basic:8.9",
            "cmds": ROCKY_FULL_CMDS
        },
        #"malt/rocky-indev:8.9": {
        #    "base": "malt/rocky-full:8.9",
        #    "cmds": ROCKY_INDEV_CMDS
        #},
        ############ rocky:9.3
        #"malt/rocky-basic:9.3": {
        #    "base": "rockylinux:9.3",
        #    "cmds": ROCKY_BASIC_CMDS
        #},
        #"malt/rocky-full:9.3": {
        #    "base": "malt/rocky-basic:9.3",
        #    "cmds": ROCKY_FULL_CMDS
        #},
        #"malt/rocky-indev:9.3": {
        #    "base": "malt/rocky-full:9.3",
        #    "cmds": [
        #        BUILD_CUSTOM_PYTHON
        #    ]
        #},
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
    cores = int(multiprocessing.cpu_count() / 2)
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

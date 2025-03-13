#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/build_and_test_on_distributions.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
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
COMMON_CONF_OPTIONS = "--with-python=/opt/malt-python"
############################################################
BUILD_CUSTOM_PYTHON = """cd /tmp \\
    && wget https://www.python.org/ftp/python/3.13.2/Python-3.13.2.tar.xz \\
    && tar -xf Python-3.13.2.tar.xz \\
    && cd Python-3.13.2 \\
    && ./configure --enable-shared --prefix=/opt/malt-python \\
    && make -j8 \\
    && make install \\
    && cd .. \\
    && rm -rfd Python-3.13.2"""
############################################################
UBUNTU_BASIC_CMDS=[
    "apt update",
    "apt upgrade -y",
    "apt install -y cmake g++ make clang",
    "apt install -y ccache",
]
UBUNTU_FULL_CMDS=[
    "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
    "apt install -y libqt5webkit5-dev",
    "apt install -y wget",
    BUILD_CUSTOM_PYTHON,
    "apt update && apt install -y nlohmann-json3-dev"
]
CENTOS_BASIC_CMDS=[
    "dnf makecache --refresh",
    "dnf update -y",
    "dnf install -y cmake gcc-c++ make clang",
    "dnf install -y ccache"
]
CENTOS_FULL_CMDS=[
    "dnf install -y libunwind-devel elfutils-libelf-devel libunwind-devel nodejs npm",
    "dnf install -y qt5-qtwebkit-devel",
    "dnf install -y wget",
    BUILD_CUSTOM_PYTHON,
    "dnf update && apt install -y nlohmann-json3-devel"
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
        ############ ubuntu:24.04
        "malt/ubuntu-basic:24.04": {
            "base": "ubuntu:24.04",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:24.04": {
            "base": "malt/ubuntu-basic:24.04",
            "cmds": UBUNTU_FULL_CMDS
        },
        ############ ubuntu:22.10
        "malt/ubuntu-basic:24.10": {
            "base": "ubuntu:24.10",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:24.10": {
            "base": "malt/ubuntu-basic:24.10",
            "cmds": UBUNTU_FULL_CMDS
        },
        ############ ubuntu:25.04
        "malt/ubuntu-basic:25.04": {
            "base": "ubuntu:25.04",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/ubuntu-full:25.04": {
            "base": "malt/ubuntu-basic:25.04",
            "cmds": UBUNTU_FULL_CMDS
        },
        ############ debian:10
        "malt/debian-basic:10": {
            "base": "debian:10",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:10": {
            "base": "malt/debian-basic:10",
            "cmds": UBUNTU_FULL_CMDS
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
        ############ debian:12
        "malt/debian-basic:12": {
            "base": "debian:12",
            "cmds": UBUNTU_BASIC_CMDS
        },
        "malt/debian-full:12": {
            "base": "malt/debian-basic:12",
            "cmds": UBUNTU_FULL_CMDS
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
        ############ almalinux:8.4
        "malt/almalinux-basic:8.4": {
            "base": "almalinux:8.4",
            "cmds": CENTOS_BASIC_CMDS
        },
        "malt/almalinux-full:8.4": {
            "base": "malt/almalinux-basic:8.4",
            "cmds": CENTOS_FULL_CMDS
        },
    },
    'compilers': {
        "gcc": "CXX=g++ CC=gcc",
        "clang": "CXX=clang++ CC=clang",
    },
    "variants": {
        "debug": "--enable-debug",
        "release": ""
    }
}

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
    container.build(base_name)

############################################################
@pytest.mark.parametrize("dist_name_version, compiler, variant", gen_distr_paramatrized())
def test_distribution(dist_name_version: str, compiler:str, variant:str):
    # extract options
    compiler_options = BUILD_PARAMETERS['compilers'][compiler]
    variant_options = BUILD_PARAMETERS['variants'][variant]

    # infos
    cores = multiprocessing.cpu_count()

    # build & start container to run commands in
    with in_container(dist_name_version) as container:
        # to perform tests
        container.assert_run(f"/mnt/malt-sources/configure --enable-tests {COMMON_CONF_OPTIONS} {variant_options} {compiler_options}")
        container.assert_run(f"make -j{cores}")
        container.assert_run(f"ctest --output-on-failure -j{cores}")

############################################################
def test_current_host_debug_no_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = multiprocessing.cpu_count()

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-debug CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command(f"make -j{cores}")

############################################################
def test_current_host_debug_disable_tests():
    # get malt source path
    sources = get_malt_source_path()

    # infos
    cores = multiprocessing.cpu_count()

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

    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            assert_shell_command(f"{sources}/configure --enable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command("make -j8")
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

    # self test oursef
    pytest.main(sys.argv)

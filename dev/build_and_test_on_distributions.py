#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
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
./build_and_test_on_distributions.p -v

# for debugging in real time, get direct output of commands.
./build_and_test_on_distributions.p -v --capture=no

# you can also call it via pytest command itself
pytest ./build_and_test_on_distributions.p
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
BUILD_PARAMETERS = {
    "distributions": {
        "ubuntu:22.04": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
        "ubuntu:23.04": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
        "ubuntu:24.04": [
            "apt update -y",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
        "debian:10": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
        "debian:11": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
        "debian:12": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make clang",
            "apt install -y libunwind-dev libelf-dev libunwind-dev nodejs npm",
            "apt install -y libqt5webkit5-dev",
            "apt install -y ccache"
        ],
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
    distr_install_cmds = BUILD_PARAMETERS['distributions'][dist_name_version]

    # build
    container = PodmanContainerHandler(f"malt/{dist_name_version}")
    container.add_build_run_rules(distr_install_cmds)
    container.build(dist_name_version)

############################################################
@pytest.mark.parametrize("dist_name_version, compiler, variant", gen_distr_paramatrized())
def test_distribution(dist_name_version: str, compiler:str, variant:str):
    # extract options
    compiler_options = BUILD_PARAMETERS['compilers'][compiler]
    variant_options = BUILD_PARAMETERS['variants'][variant]

    # infos
    cores = multiprocessing.cpu_count()

    # build & start container to run commands in
    with in_container(f"malt/{dist_name_version}") as container:
        # to perform tests
        container.assert_run(f"/mnt/malt-sources/configure --enable-tests {variant_options} {compiler_options}")
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
    # nicer to get -v by default to get the list of tests displayed as 'integration tests' can be long
    sys.argv.append('-v')

    # we are not testing pythong thing itself, so no need to get the header
    sys.argv.append('--no-header')

    # self test oursef
    pytest.main(sys.argv)

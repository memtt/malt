#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 03/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/build_and_test_on_distributions.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
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
import os
import sys
import shutil
import subprocess
import tempfile
import multiprocessing
from contextlib import contextmanager
# pytest
import pytest

############################################################
@contextmanager
def jump_in_dir(path: str, erase_first = False, create = False):
    '''
    An helper semantic to jump temporarily in a directory. As wrapped with python 'with' semantic,
    went back automatically on the previous one on error or success.
    '''
    # get old path
    oldpwd = os.getcwd()

    # remove & recrete if asked
    if erase_first and os.path.exists(path):
       shutil.rmtree(path)
    if create:
       os.makedirs(path, exist_ok=True)

    # chump in
    os.chdir(path)

    # handle return back afterward
    try:
        yield
    finally:
        os.chdir(oldpwd)

############################################################
def assert_shell_command(command: str) -> None:
    '''
    An helper wrapper to just call a command and check status.

    Note: For the usage we want we got a smaller and cleaner message on failure instead of getting
    dump of the internal of subprocess.run(..., check=True) on failure which we can also have
    called directly to get same behavior.
    '''
    status = subprocess.run(command, shell=True)
    if status.returncode != 0:
        raise Exception(f"Command failure (status={status.returncode}) while running : {command}")

############################################################
def get_malt_source_path() -> str:
    return os.path.abspath(os.path.join(__file__, '..', '..'))

###########################################################
class PodmanContainerHandler:
    def __init__(self, image: str):
        self.check_podman()
        self.image = image
        self.build_rules = []
        self.container_id = None

    @staticmethod
    def check_podman() -> None:
        if not shutil.which("podman"):
            raise Exception("'podman' command is missing !")

    def add_build_run_rule(self, rule: str) -> None:
        self.build_rules.append(f"RUN {rule}")

    def add_build_run_rules(self, rules: list) -> None:
        for rule in rules:
            self.build_rules.append(f"RUN {rule}")

    def build(self, base_image: str) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            with open(f"{tmpdir}/Dockerfile", "w+") as fp:
                # build Dockerfile
                cmds = [f"FROM {base_image}"]
                cmds += self.build_rules

                # store it
                fp.write("\n".join(cmds))
                fp.flush()

                # build image
                assert_shell_command(f"podman build --tag {self.image} {tmpdir}")

    def start(self):
        host_sources_path = get_malt_source_path()
        cmd = ['podman', 'run', '--detach', '--volume', f'{host_sources_path}:/mnt/malt-sources:rw', '--rm', '--tty', self.image, "/bin/bash"]
        print(f"Running : {cmd}")
        self.container_id = subprocess.check_output(cmd).decode().split('\n')[0]
        print(self.container_id)
        self.assert_run("mkdir /home/build", in_build=False)

    def assert_run(self, command: str, in_build = True):
        bash_command = f"/bin/bash -c \"{command}\""
        print("")
        print(f"- {bash_command}")
        status: subprocess.CompletedProcess
        if in_build:
            cmd = ['podman', 'exec', '-it', '-w', "/home/build", self.container_id, "/bin/bash", "-c", command]
        else:
            cmd = ['podman', 'exec', '-it',                      self.container_id, "/bin/bash", "-c", command]
        print(f"Running {cmd}")
        status = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        msg = status.stdout.decode('utf-8')
        output_dump = f"+++++++++++++++++++++++++++++++\n{msg}\n+++++++++++++++++++++++++++++++\n"
        #print(output_dump)
        if status.returncode != 0:
            raise Exception(f"Fail to execute in image : {self.image}, command : {command}\n{output_dump}")

    def stop(self):
        if self.container_id:
            print(self.container_id)
            assert_shell_command(f'podman stop {self.container_id}')

############################################################
@contextmanager
def in_container(image: str):
    # handle return back afterward
    try:
        # create container handler
        handler = PodmanContainerHandler(image)

        # start
        handler.start()

        # make action
        yield handler
    finally:
        handler.stop()

############################################################
BUILD_PARAMETERS = {
    "distributions": {
        "ubuntu:22.04": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "ubuntu:23.04": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "ubuntu:23.10": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "ubuntu:24.04": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "debian:10": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "debian:11": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
        ],
        "debian:12": [
            "apt update",
            "apt upgrade -y",
            "apt install -y cmake g++ make libunwind-dev nodejs clang"
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
            assert_shell_command(f"make --output-on-failure -j{cores}")

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
# To be able to run as a standard program directly and not call via pytest command
# (can still also)
if __name__ == '__main__':
    # nicer to get -v by default to get the list of tests displayed as 'integration tests' can be long
    sys.argv.append('-v')

    # we are not testing pythong thing itself, so no need to get the header
    sys.argv.append('--no-header')

    # self test oursef
    pytest.main(sys.argv)

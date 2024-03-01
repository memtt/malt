#!/usr/bin/env python3
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Sebastien Valat              #
#            LICENSE  : Apache 2.0                   #
######################################################

######################################################
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

######################################################
# python
import os
import sys
import shutil
import subprocess
import tempfile
from contextlib import contextmanager
# docker
import docker
# pytest
import pytest

###########################################################
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

###########################################################
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

###########################################################
def get_malt_source_path() -> str:
    return os.path.abspath(os.path.join(__file__, '..', '..'))

###########################################################
class ContainerHandler:
    def __init__(self, image: str):
        self.image = image
        self.client = docker.from_env()
        self.build_rules = []

    def add_build_run_rule(self, rule: str) -> None:
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
                self.client.images.build(path=tmpdir, tag=self.image)

    def start(self):
        host_sources_path = get_malt_source_path()
        volumes = {
            host_sources_path: {'bind': '/mnt/malt-sources', 'mode': 'rw'}
        }
        self.container = self.client.containers.run(self.image, "/bin/bash", volumes=volumes, remove=True, detach=True, tty=True)
        self.assert_run("mkdir /home/build", in_build=False)

    def assert_run(self, command: str, in_build = True):
        bash_command = f"/bin/bash -c \"{command}\""
        print("")
        print(f"- {bash_command}")
        if in_build:
            status = self.container.exec_run(bash_command, workdir="/home/build")
        else:
            status = self.container.exec_run(bash_command)
        msg = status.output.decode('utf-8')
        output_dump = f"+++++++++++++++++++++++++++++++\n{msg}\n+++++++++++++++++++++++++++++++\n"
        #print(output_dump)
        if status.exit_code != 0:
            raise Exception(f"Fail to execute in image : {self.image}, command : {command}\n{output_dump}")

    def stop(self):
        if self.container:
            self.container.stop()
        self.client.close()

###########################################################
@contextmanager
def in_container(image: str):
    # handle return back afterward
    try:
        handler = ContainerHandler(image)
        yield handler
    finally:
        handler.stop()

######################################################
@pytest.mark.parametrize("dist_name_version", ["ubuntu:20.04", "ubuntu:22.04", "ubuntu:23.04", "ubuntu:24.04", "debian:10", "debian:11", "debian:12"])
def test_distribution(dist_name_version: str):
    with in_container(f"malt/{dist_name_version}") as container:
        # to build the container if not exist yet
        container.add_build_run_rule("apt update")
        container.add_build_run_rule("apt install -y cmake g++ make libunwind-dev nodejs")

        # build & start
        container.build(dist_name_version)
        container.start()

        # to perform tests
        container.assert_run("/mnt/malt-sources/configure --enable-debug --enable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
        container.assert_run("make -j8")
        container.assert_run("make test")

######################################################
def test_current_host_debug_no_tests():
    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            sources = get_malt_source_path()
            assert_shell_command(f"{sources}/configure --enable-debug CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command("make -j8")

######################################################
def test_current_host_debug_disable_tests():
    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            sources = get_malt_source_path()
            assert_shell_command(f"{sources}/configure --enable-debug --disable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command("make -j8")

######################################################
def test_current_host_debug_tests():
    with tempfile.TemporaryDirectory() as tmpdir:
        with jump_in_dir(tmpdir):
            sources = get_malt_source_path()
            assert_shell_command(f"{sources}/configure --enable-debug --enable-tests CFLAGS=-Werror CXXFLAGS=-Werror")
            assert_shell_command("make -j8")
            assert_shell_command("make test")

######################################################
# To be able to run as a standard program directly and not call via pytest command
# (can still also)
if __name__ == '__main__':
    # nicer to get -v by default to get the list of tests displayed as 'integration tests' can be long
    sys.argv.append('-v')

    # we are not testing pythong thing itself, so no need to get the header
    sys.argv.append('--no-header')

    # self test oursef
    pytest.main(sys.argv)

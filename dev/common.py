############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/common.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import sys
import shutil
import traceback
import tempfile
import subprocess
from contextlib import contextmanager
from termcolor import cprint

############################################################
def get_malt_source_path() -> str:
    return os.path.abspath(os.path.join(__file__, '..', '..'))

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

##########################################################
def print_exception(exception: Exception) -> None:
    dir = os.getcwd()
    cprint("-----------------------------------------------", 'red')
    cprint(''.join(traceback.format_exception(exception)), 'grey')
    cprint("-----------------------------------------------", 'red')
    cprint(f"Error from command : {' '.join(sys.argv)}", 'red')
    cprint(f"Error from workdir : {dir}", 'red')
    cprint("-----------------------------------------------", 'red')
    cprint(str(exception), 'red')
    cprint("-----------------------------------------------", 'red')
    print("")

############################################################
# Run command after logging it and intercept error
def run_shell(cmd: str) -> None:
    cprint(f"  + {cmd}", "cyan")
    assert_shell_command(cmd)

############################################################
def run_shell_cmds(cmds: list) -> None:
    for cmd in cmds:
        run_shell(cmd)

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
@contextmanager
def jump_in_tmpdir():
    tmpdir = tempfile.mkdtemp()
    try:
        with jump_in_dir(tmpdir):
            yield tmpdir
    finally:
        shutil.rmtree(tmpdir)

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

    def build_cache_volumes(self, caches:dict) -> list:
        # sources
        volumes = []

        # apply
        for key, vm_path in caches.items():
            img = self.image.replace(":", "_")
            dir = key.replace("@IMG@", img)
            os.makedirs(dir, exist_ok=True)
            volumes.append("--volume")
            volumes.append(f"{os.getcwd()}/{dir}:{vm_path}:rw")

        # ok
        return volumes

    def build(self, base_image: str, caches: dict = {}) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            with open(f"{tmpdir}/Dockerfile", "w+") as fp:
                # build Dockerfile
                cmds  = [f"FROM {base_image}"]
                cmds += [
                    "ENV DEBIAN_FRONTEND=noninteractive",
                    'ENV PATH "/usr/lib/ccache:/usr/lib/ccache/bin:$PATH"'
                ]
                cmds += self.build_rules

                # store it
                fp.write("\n".join(cmds))
                fp.flush()

                # cmd  
                cmd = ["podman",
                       "build",
                ] + self.build_cache_volumes(caches) + [
                       "--tag",
                       self.image,
                       tmpdir
                ]

                # build image
                assert_shell_command(' '.join(cmd))

    def start(self, caches: dict={}):
        host_sources_path = get_malt_source_path()

        cmd = ['podman', 'run', 
               '--detach',
               '--volume', f'{host_sources_path}:/mnt/malt-sources:rw',
        ] + self.build_cache_volumes(caches) + [
               '--rm',
               '--tty',
               self.image,
               "/bin/bash"
        ]
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
def in_container(image: str, caches: dict = {}):
    # handle return back afterward
    try:
        # create container handler
        handler = PodmanContainerHandler(image)

        # start
        handler.start(caches=caches)

        # make action
        yield handler
    finally:
        handler.stop()

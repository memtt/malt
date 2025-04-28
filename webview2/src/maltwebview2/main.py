import sys
import os
import argparse
import subprocess
import threading
import signal
import sys
from .apizmq import MaltProfileRequest

def malt_profile_zqm(file):
    dir = os.path.dirname(os.path.abspath(__file__))
    subprocess.run([
        "xterm", "-e", "gdb", "--args",
        "./src/reader/libreader/malt-zmq-reader",
        file
    ], cwd=os.path.abspath(f"{dir}/../../../../../build"))
    print("KILLLED ==============")

def fastapi_process(file):
    dir = os.path.dirname(os.path.abspath(__file__))

    env = os.environ.copy()
    env['MALT_WEBVIEW2_FILE'] = file
    env['MALT_WEBVIEW2_SOCKET'] = "ipc:///tmp/socket"
    if not os.path.exists(file) or not os.path.isfile(file):
        print(f"[MALT-WEBVIEW2] Invalid file : {file}", file=sys.stderr)
        return -1

    subprocess.run([
        "fastapi",
        "dev",
        "--port", "8080",
        f"{dir}/serv.py"
    ], cwd=os.path.abspath(f"{dir}/../../../../webview/"), env=env, check=False)

def main():
    # decl parser
    parser = argparse.ArgumentParser("malt-webview2", description="The new webview for MALT !")
    parser.add_argument("FILE", help="The MALT file profile to load under JSON form.")
    
    # parse
    args = parser.parse_args()
    
    # set env
    file = os.path.abspath(args.FILE)

    print("---------- Starting ZMQ server -----------")
    malt_zmq_server_thread = threading.Thread(target=malt_profile_zqm, args=[file])
    malt_zmq_server_thread.start()

    serv = MaltProfileRequest("ipc:///tmp/socket")
    serv.wait_ready()

    print("------- Starting FastAPI server ----------")
    fastapi_thread = threading.Thread(target=fastapi_process, args=[file])
    fastapi_thread.start()

    print("----------------- Stopping all -----------")
    fastapi_thread.join()
    serv.stop()
    malt_zmq_server_thread.join()

    # ok
    return 0

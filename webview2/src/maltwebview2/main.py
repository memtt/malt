import sys
import os
import argparse
import subprocess
import threading
from .apizmq import MaltProfileRequest

def malt_profile_zqm(file):
    subprocess.run([
        "./src/reader/libreader/malt-zmq-reader",
        file
    ], cwd=os.path.abspath(f"{dir}/../../build-release"))

def main():
    # decl parser
    parser = argparse.ArgumentParser("malt-webview2", description="The new webview for MALT !")
    parser.add_argument("FILE", help="The MALT file profile to load under JSON form.")
    
    # parse
    args = parser.parse_args()
    
    # set env
    env = os.environ.copy()
    file = os.path.abspath(args.FILE)
    env['MALT_WEBVIEW2_FILE'] = file
    env['MALT_WEBVIEW2_SOCKET'] = "ipc:///tmp/socket"
    if not os.path.exists(file) or not os.path.isfile(file):
        print(f"[MALT-WEBVIEW2] Invalid file : {file}", file=sys.stderr)
        return -1

    print("Starting ZMQ server")
    event_watcher_ts = threading.Thread(target=malt_profile_zqm, args=[file])
    event_watcher_ts.start()

    serv = MaltProfileRequest("ipc:///tmp/socket")
    serv.wait_ready()

    dir = os.path.dirname(os.path.abspath(__file__))
    subprocess.run([
        "fastapi",
        "dev",
        "--port", "8080",
        f"{dir}/serv.py"
    ], cwd=os.path.abspath(f"{dir}/../../../../webview/"), env=env)

    # ok
    return 0

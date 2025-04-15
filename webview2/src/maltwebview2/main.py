import os
import argparse
import subprocess

def main():
    # decl parser
    parser = argparse.ArgumentParser("malt-webview2", description="The new webview for MALT !")
    parser.add_argument("FILE", help="The MALT file profile to load under JSON form.")
    
    # parse
    args = parser.parse_args()
    
    # set env
    env = os.environ.copy()
    env['MALT_WEBVIEW2_FILE'] = os.path.abspath(args.FILE)
    
    dir = os.path.dirname(os.path.abspath(__file__))
    subprocess.run([
        "fastapi",
        "dev",
        f"{dir}/serv.py"
    ], cwd=f"{dir}/../../../../webview/", env=env)

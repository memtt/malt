#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : ./src/reader/webview-2-python/checker.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import subprocess
import threading
import urllib.request
import json 
import jsondiff
import timeit
import urllib
import collections

############################################################
def order_dict(dictionary):
    return {k: order_dict(v) if isinstance(v, dict) else v
            for k, v in sorted(dictionary.items())}

############################################################
def check(entry_point: str, data = None, sort = False) -> None:
    req = urllib.request.Request(f"http://localhost:8080/{entry_point}", data=data)
    req.add_header('Content-Type', 'application/json')
    with urllib.request.urlopen(req) as url:
        data1 = json.load(url)
    req2 = urllib.request.Request(f"http://localhost:8081/{entry_point}", data=data)
    req2.add_header('Content-Type', 'application/json')
    with urllib.request.urlopen(req2) as url:
        data2 = json.load(url)
    if sort:
        print(data1)
        data1 = sorted(data1, key=lambda entry: (entry["file"] + ":" + entry["function"] + ":" + str(entry["line"])))
        data2 = sorted(data2, key=lambda entry: (entry["file"] + ":" + entry["function"] + ":" + str(entry["line"])))
    diff = jsondiff.diff(data1, data2, syntax='explicit')
    if diff:
        print(f"{entry_point} -- HAS DIFF -- {diff}")
    else:
        print(f"{entry_point} -- OK")
    result1 = timeit.timeit(f"import urllib\nlocal_filename, headers = urllib.request.urlretrieve('http://localhost:8080/{entry_point}')", number=10)
    result2 = timeit.timeit(f"import urllib\nlocal_filename, headers = urllib.request.urlretrieve('http://localhost:8081/{entry_point}')", number=10)
    print(f"{entry_point} -- {result1}")
    print(f"{entry_point} -- {result2}")
    if sort:
        print("-------------------------------")
        print(len(data1[0]))
        print(len(data2[0]))
        print("-------------------------------")
        print(order_dict(data1[0]))
        print("-------------------------------")
        print(order_dict(data2[0]))
        print("-------------------------------")

############################################################
def main() -> None:
    check("summary.json")
    check("data/summary.json")
    check("flat.json", sort=True)
    check("call-stack-next-level.json", data='{"parentStackDepth": 0, "parentStackId": 0}'.encode('utf-8') )

############################################################
if __name__ == "__main__":
    main()

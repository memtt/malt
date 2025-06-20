#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : examples/python/step-7-threads-not-yet-well-supported.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
from threading import Thread
import time

############################################################
def thread_main(count, id):
	print(f"Start thread...{id}")
	a = [10] * int(count / 2)
	time.sleep(5)
	for i in range(int(count / 2)):
		a.append(20)
	print(f"ok {id} : {a[0:200]} .. {a[-200:]}")
	print(f"End thread...{id}")

############################################################
print("SPAWN THREADS...")
t1 = Thread(target=thread_main, args=(1024*1024,1,))
t1.start()
t2 = Thread(target=thread_main, args=(1024*1024,2,))
t2.start()

############################################################
t1.join()
t2.join()
print("END")

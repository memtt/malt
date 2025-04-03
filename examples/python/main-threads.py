from threading import Thread
import time

def thread_main(count):
	print("start")
	print(count)
	a = [10] * int(count / 2)
	time.sleep(5)
	for i in range(int(count / 2)):
		a.append(20)
	print("ok")
	print(a)
	print("ok")

t1 = Thread(target=thread_main, args=(1024*1024,))
t1.start()
t2 = Thread(target=thread_main, args=(1024*1024,))
t2.start()

t1.join()
t2.join()
print("END")

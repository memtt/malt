import gc

def main():
    a = []
    i = 0
    for k in range(100000):
        a.append(k)
    arr = [0 for i in range(100000)]

gc.disable()
main()
main()
main()


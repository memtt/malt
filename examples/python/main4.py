import numpy
def main():
	N = 100000
	a = [None] * N
	for i in range(N):
		a[i] = 25.5
	b = []
	for i in range(N):
		b.append(25.5)
	c = numpy.array([0]*N)
main()

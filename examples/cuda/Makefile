NVCC=nvcc

all: vecadd

vecadd: vecadd.cu
	$(NVCC) -cudart shared -g -o vecadd vecadd.cu

clean:
	rm -f vecadd

.PHONY: clean

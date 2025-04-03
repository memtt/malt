all:
	ifort -O2 -g -finstrument-functions -heap-arrays -o reproducer_heap main.f90
	ifort -O2 -g -finstrument-functions -o reproducer_stack main.f90

test:
	malt ./reproducer_heap
	malt ./reproducer_stack

clean:
	rm -f reproducer_heap reproducer_stack
	rm -f malt-*

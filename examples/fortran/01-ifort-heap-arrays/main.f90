! declare a routing with arrays inside
subroutine use_memory( n )
	! decalre
	integer :: n
	real :: x ( n )     ! an automatic array
	real :: z ( n )     ! an automatic array
	real :: y ( 1000 )  ! an explicit-shape local array on the stack

	! init
	x = 1.0
	y = 2.0
	z = 5.0

	! make computation
	z = x + y(20)

	! print
	print *, 'Array = ', sum(z)
end subroutine

program reproducer
	call use_memory( 100 )
	call use_memory( 100*1024*1024 )
end program reproducer

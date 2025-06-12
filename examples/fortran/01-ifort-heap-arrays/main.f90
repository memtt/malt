!-----------------------------------------------------------
!    PROJECT  : MALT (MALoc Tracker)
!    VERSION  : 1.2.6
!    DATE     : 04/2025
!    LICENSE  : CeCILL-C
!    FILE     : examples/fortran/01-ifort-heap-arrays/main.f90
!-----------------------------------------------------------
!    AUTHOR   : Sébastien Valat (INRIA) - 2025
!----------------------------------------------------------!
! declare a routing with arrays inside
subroutine use_memory( n )
	! decalre
	integer :: n
	real :: x ( n )     ! an automatic array
	real :: z ( n )     ! an automatic array
	real :: y ( 1000 )  ! an explicit-shape local array on the stack
	integer, parameter :: fixed_n = 100 * 1024 * 1024 ! for fixed size array
	real :: zz ( fixed_n ) ! A fixed size array

	! init
	x = 1.0
	y = 2.0
	z = 5.0

	! make computation
	z = x + y(20)

	! copy
	do i=1,n
		zz(i) = z(i) + 1.0
	end do

	! print
	print *, 'Array = ', sum(z) + sum(zz)
end subroutine

program reproducer
	call use_memory( 100 )
	call use_memory( 100*1024*1024 )
end program reproducer

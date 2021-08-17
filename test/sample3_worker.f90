program test1_req
  use mpi
  use ctca

  implicit none
  integer(kind=4), parameter :: n = 1000
  integer(kind=4) :: dataintnum, myrank, nprocs, ierr, fromrank
  integer(kind=4),dimension(1) :: dataint
  real(kind=8),dimension(:),allocatable :: data
  real(kind=8),dimension(n, n) :: a
  real(kind=8) :: r, val
  integer(kind=4) :: i, j, m, w
  character(len=32) :: str

  m=128
  w=8

  call CTCAW_init(0, w)

  do i = 1, n
     do j = 1, n
        call random_number(val)
        a(j, i) = val - 0.5
     end do
  end do

  allocate(data(m/8))

  call MPI_Comm_size(CTCA_subcomm, nprocs, ierr)
  call MPI_Comm_rank(CTCA_subcomm, myrank, ierr)

  call ctcaw_prof_start()
  do while (.true.)
     call CTCAW_pollreq_withreal8(fromrank, dataint, 1, data, m/8)

     if (CTCAW_isfin()) then
        exit
     end if

     call ctcaw_prof_start_calc()
     call calc(n, a, r, data(1))
     call ctcaw_prof_stop_calc()

     call CTCAW_complete()
  end do
  call ctcaw_prof_stop()

  call CTCAW_finalize()

  deallocate(data)

  print *, "Worker result:", r

end program

subroutine calc(n, a, r, data)
  implicit none
  integer, intent(in) :: n
  real(kind=8), intent(out) :: r
  real(kind=8), intent(in) :: data
  real(kind=8), dimension(n, n) :: a
  real(kind=8), dimension(n) :: b
  integer :: i, j
  real(kind=8) :: val
  
  b(:) = 0.0
  
  do i = 1, n
     do j = 1, n
        b(i) = b(i) + a(j, i) * data
     end do
  end do
  r = sum(b(:))
  !print *, sum(data(:))

end subroutine


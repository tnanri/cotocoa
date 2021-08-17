program test1_req
  use mpi
  use ctca
  implicit none
  integer(kind=4) :: m, n, r, nprocs, myrank, i, j, jobid, ierr, rval, rtotal, repeat
  character(len=32) :: str
  real(kind=8), dimension(:), allocatable :: data
  integer(kind=8), dimension(:), allocatable :: hdl
  integer(kind=4), dimension(1) :: val
  real(kind=8) :: x

  m=128
  n=20
  r=1
  repeat=100

  call CTCAR_init_detail(10, n, 10)

  call MPI_Comm_size(CTCA_subcomm, nprocs, ierr)
  call MPI_Comm_rank(CTCA_subcomm, myrank, ierr)

  allocate(data(m/8))
  allocate(hdl(n))
  do i = 0, m/8
     call random_number(x)
     data(i) = x - 0.5
  end do

  call ctcar_prof_start()
  if (myrank < r) then
     jobid = repeat * myrank;
     rtotal = 0.0

     do i = 1, repeat
        call ctcar_prof_start_total()
        do j = 1, n
           val(1) = jobid
           call CTCAR_sendreq_withreal8_hdl(val, 1, data, m/8, hdl(j))
           jobid = jobid + 1
        end do

        call ctcar_prof_start_calc()
        call calc(rval)
        rtotal = rtotal + rval
        call ctcar_prof_stop_calc()

        do j = 1, n
           call CTCAR_wait(hdl(j))
        end do
        call ctcar_prof_stop_total()
     end do
  end if
  call ctcar_prof_stop()

  print *, "Data size , ", m
  print *, "Requests per repeat , ", n
  print *, "Num requesters , ", r
  print *, "Num repeats , ", repeat

  call CTCAR_finalize()

  deallocate(data)
  deallocate(hdl)

end program

subroutine calc(r)
implicit none
real(kind=8), intent(out) :: r
integer :: i
real(kind=8) :: t, h

t = 0.0
do i = 1, 10000
   call random_number(h)
   t = t + h
end do

r = t
!print *, t

end subroutine


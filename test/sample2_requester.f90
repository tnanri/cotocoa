program sample_requester
  use mpi
  use ctca
  implicit none

  integer(kind=4) :: dataintnum, myrank, nprocs, ierr, areaid, progid, fromrank
  integer(kind=4),dimension(4) :: reqinfo
  integer(kind=4),dimension(2) :: dataint
  real(kind=8),dimension(6,400,10) :: datareal8
  integer(kind=4) :: i, j, k
  integer(kind=4), parameter :: prognum = 3

  call CTCAR_init()
  print *, "requester init done"

  call MPI_Comm_size(CTCA_subcomm, nprocs, ierr)
  call MPI_Comm_rank(CTCA_subcomm, myrank, ierr)

  call CTCAR_regarea_real8(datareal8, 6*400*10, areaid)

  do i = 1, 10
     progid = mod(i, prognum) + 1
     dataint(1) = progid
     dataint(2) = i

     do j = 1, 400
        do k = 1, 6
           datareal8(k, j, i) = i*10000 + j*10 + k
        end do
     end do
     if (myrank == 0) then
  print *, "requester sendreq : ", i
        call CTCAR_sendreq(dataint, 2)
  print *, "requester sendreq done"
     end if
  end do

  call CTCAR_finalize()

end program


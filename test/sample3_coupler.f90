program test1_cpl
  use mpi
  use ctca
  implicit none
  integer(kind=4), dimension(1) :: dataint
  real(kind=8), dimension(:), allocatable :: data
  integer(kind=4), dimension(4) :: reqinfo
  integer(kind=4) :: fromrank, m, n
  character(len=32) :: str

  m=128
  n=20

  call CTCAC_init_detail(1, n+10, 10, m*2, n+10)

  call ctcac_prof_start()

  allocate(data(m/8))

  do while (.true.)
     call CTCAC_pollreq_withreal8(reqinfo, fromrank, dataint, 1, data, m/8)

     if (CTCAC_isfin()) then
        exit
     end if

     if (fromrank >= 0) then
        ! enqueue request with data
        call CTCAC_enqreq_withreal8(reqinfo, 0, dataint, 1, data, m/8)
     end if
  end do

  call ctcac_prof_stop()
  call CTCAC_finalize()

  deallocate(data)

end program

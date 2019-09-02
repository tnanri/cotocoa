module ctca
  use, intrinsic :: iso_c_binding
  use mpi
  implicit none

  interface
     function ctca_get_subcomm() bind(c, name='CTCA_get_subcomm')
       import
       integer(kind=4) :: ctca_get_subcomm
     end function ctca_get_subcomm

     function ctca_get_reqinfoitems() bind(c, name='CTCA_get_reqinfoitems')
       import
       integer(kind=4) :: ctca_get_reqinfoitems
     end function ctca_get_reqinfoitems

     subroutine ctcar_init_f() bind(c, name='ctcar_init_f')
       import
     end subroutine ctcar_init_f

     subroutine ctcar_init_detail_f(numareas, numreqs, intparams) bind(c, name='ctcar_init_detail_f')
       import
       integer(kind=4) :: numareas, numreqs, intparams
     end subroutine ctcar_init_detail_f

     subroutine ctcar_regarea_int(base, size, areaid) bind(c, name='ctcar_regarea_int_f')
       import
       integer(kind=4) :: size, areaid
       integer(kind=4), dimension(1:size) :: base
     end subroutine ctcar_regarea_int

     subroutine ctcar_regarea_real4(base, size, areaid) bind(c, name='ctcar_regarea_real4_f')
       import
       integer(kind=4) :: size, areaid
       real(kind=4), dimension(1:size) :: base
     end subroutine ctcar_regarea_real4

     subroutine ctcar_regarea_real8(base, size, areaid) bind(c, name='ctcar_regarea_real8_f')
       import
       integer(kind=4) :: size, areaid
       real(kind=8), dimension(1:size) :: base
     end subroutine ctcar_regarea_real8

     subroutine ctcar_sendreq(intparams, numintparams) bind(c, name='ctcar_sendreq_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=4), dimension(1:numintparams) :: intparams
     end subroutine ctcar_sendreq

     subroutine ctcar_sendreq_hdl(intparams, numintparams, hdl) bind(c, name='ctcar_sendreq_hdl_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=4), dimension(1:numintparams) :: intparams
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_hdl

     subroutine ctcar_sendreq_withint(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withint_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withint

     subroutine ctcar_sendreq_withreal4(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withreal4_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withreal4

     subroutine ctcar_sendreq_withreal8(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withreal8_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withreal8

     subroutine ctcar_sendreq_withint_hdl(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withint_hdl_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       integer(kind=4), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withint_hdl

     subroutine ctcar_sendreq_withreal4_hdl(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withreal4_hdl_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=4), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withreal4_hdl

     subroutine ctcar_sendreq_withreal8_hdl(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withreal8_hdl_f')
       import
       integer(kind=4) :: numintparams, datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=8), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withreal8_hdl

     subroutine ctcar_wait(hdl) bind(c, name='ctcar_wait_f')
       import
       integer(kind=8) :: hdl
     end subroutine ctcar_wait

     subroutine ctcar_finalize() bind(c, name='ctcar_finalize_f')
       import
     end subroutine ctcar_finalize

     subroutine ctcac_init_f() bind(c, name='ctcac_init_f')
       import
     end subroutine ctcac_init_f

     subroutine ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz, bufslotnum) &
          bind(c, name='ctcac_init_detail_f')
       import
       integer(kind=4) :: numareas, numreqs, intparams, bufslotsz, bufslotnum
     end subroutine ctcac_init_detail_f

     subroutine ctcac_regarea_int(areaid) bind(c, name='ctcac_regarea_int_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcac_regarea_int

     subroutine ctcac_regarea_real4(areaid) bind(c, name='ctcac_regarea_real4_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcac_regarea_real4

     subroutine ctcac_regarea_real8(areaid) bind(c, name='ctcac_regarea_real8_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcac_regarea_real8

     function ctcac_isfin_f() bind(c, name='ctcac_isfin_f')
       import
       integer(kind=4) :: ctcac_isfin_f
     end function ctcac_isfin_f

     subroutine ctcac_pollreq(reqinfo, fromrank, intparam, intparamnum) bind(c, name='ctcac_pollreq_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
     end subroutine ctcac_pollreq

     subroutine ctcac_pollreq_withint(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withint_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withint

     subroutine ctcac_pollreq_withreal4(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withrealr_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withreal4

     subroutine ctcac_pollreq_withreal8(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withreal8_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withreal8

     subroutine ctcac_enqreq(reqinfo, progid, intparam, intparamnum) bind(c, name='ctcac_enqreq_f')
       import
       integer(kind=4) :: progid, intparamnum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
     end subroutine ctcac_enqreq

     subroutine ctcac_enqreq_withint(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withint_f')
       import
       integer(kind=4) :: progid, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withint

     subroutine ctcac_enqreq_withreal4(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withreal4_f')
       import
       integer(kind=4) :: progid, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withreal4

     subroutine ctcac_enqreq_withreal8(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withreal8_f')
       import
       integer(kind=4) :: progid, intparamnum, datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withreal8

     subroutine ctcac_readarea_int(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_int_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       integer(kind=4), dimension(1:size) :: dest
     end subroutine ctcac_readarea_int

     subroutine ctcac_readarea_real4(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       real(kind=4), dimension(1:size) :: dest
     end subroutine ctcac_readarea_real4

     subroutine ctcac_readarea_real8(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       real(kind=8), dimension(1:size) :: dest
     end subroutine ctcac_readarea_real8

     subroutine ctcac_finalize() bind(c, name='ctcac_finalize_f')
       import
     end subroutine ctcac_finalize

     subroutine ctcaw_init_f(progid, procspercomm) bind(c, name='ctcaw_init_f')
       import
       integer(kind=4) :: progid, procspercomm
     end subroutine ctcaw_init_f

     subroutine ctcaw_init_detail_f(progid, procspercomm, numareas, intparams) bind(c, name='ctcaw_init_detail_f')
       import
       integer(kind=4) :: progid, procspercomm, numareas, intparams
     end subroutine ctcaw_init_detail_f

     subroutine ctcaw_regarea_int(areaid) bind(c, name='ctcaw_regarea_int_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcaw_regarea_int

     subroutine ctcaw_regarea_real4(areaid) bind(c, name='ctcaw_regarea_real4_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcaw_regarea_real4

     subroutine ctcaw_regarea_real8(areaid) bind(c, name='ctcaw_regarea_real8_f')
       import
       integer(kind=4) :: areaid
     end subroutine ctcaw_regarea_real8

     function ctcaw_isfin_f() bind(c, name='ctcaw_isfin_f')
       import
       integer(kind=4) :: ctcaw_isfin_f
     end function ctcaw_isfin_f

     subroutine ctcaw_pollreq(fromrank, intparam, intparamnum) bind(c, name='ctcaw_pollreq_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=4), dimension(1:intparamnum) :: intparam
     end subroutine ctcaw_pollreq

     subroutine ctcaw_pollreq_withint(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withint_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withint

     subroutine ctcaw_pollreq_withreal4(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withreal4_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withreal4

     subroutine ctcaw_pollreq_withreal8(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withreal8_f')
       import
       integer(kind=4) :: fromrank, intparamnum, datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withreal8

     subroutine ctcaw_complete() bind(c, name='ctcaw_complete_f')
       import
     end subroutine ctcaw_complete

     subroutine ctcaw_readarea_int(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_int_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       integer(kind=4), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_int

     subroutine ctcaw_readarea_real4(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       real(kind=4), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_real4

     subroutine ctcaw_readarea_real8(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank, offset, size
       real(kind=8), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_real8

     subroutine ctcaw_finalize() bind(c, name='ctcaw_finalize_f')
       import
     end subroutine ctcaw_finalize

  end interface

  integer(kind=4) :: CTCA_subcomm, CTCAC_REQINFOITEMS

  public CTCA_subcomm, CTCAC_REQINFOITEMS
  public CTCAR_init, CTCAR_init_detail
  public CTCAC_init, CTCAC_init_detail, CTCAC_isfin
  public CTCAW_init, CTCAW_init_detail, CTCAW_isfin

contains
  subroutine ctcar_init()
    implicit none
    call ctcar_init_f()
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcar_init

  subroutine ctcar_init_detail(numareas, numreqs, intparams) 
    implicit none
    integer(kind=4) :: numareas, numreqs, intparams
    call ctcar_init_detail_f(numareas, numreqs, intparams)
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcar_init_detail

  subroutine ctcac_init()
    implicit none
    call ctcac_init_f()
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcac_init

  subroutine ctcac_init_detail(numareas, numreqs, intparams, bufslotsz, bufslotnum) 
    implicit none
    integer(kind=4) :: numareas, numreqs, intparams, bufslotsz, bufslotnum
    call ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz, bufslotnum)
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcac_init_detail

  function ctcac_isfin()
    implicit none
    logical :: ctcac_isfin

    if (ctcac_isfin_f() == 1) then
       ctcac_isfin = .true.
    else
       ctcac_isfin = .false.
    end if
  end function ctcac_isfin

  subroutine ctcaw_init(progid, procspercomm)
    implicit none
    integer(kind=4) :: progid, procspercomm
    call ctcaw_init_f(progid, procspercomm)
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcaw_init

  subroutine ctcaw_init_detail(progid, procspercomm, numareas, intparams) 
    implicit none
    integer(kind=4) :: progid, procspercomm, numareas, intparams
    call ctcaw_init_detail_f(progid, procspercomm, numareas, intparams)
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcaw_init_detail

  function ctcaw_isfin()
    implicit none
    logical :: ctcaw_isfin

    if (ctcaw_isfin_f() == 1) then
       ctcaw_isfin = .true.
    else
       ctcaw_isfin = .false.
    end if
  end function ctcaw_isfin

end module ctca

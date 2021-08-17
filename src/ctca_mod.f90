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

     subroutine ctcar_regarea_int_f(base, size, areaid) bind(c, name='ctcar_regarea_int_f')
       import
       integer(kind=4) :: areaid
       integer(kind=8) :: size
       integer(kind=4), dimension(1:size) :: base
     end subroutine ctcar_regarea_int_f

     subroutine ctcar_regarea_real4_f(base, size, areaid) bind(c, name='ctcar_regarea_real4_f')
       import
       integer(kind=4) :: areaid
       integer(kind=8) :: size
       real(kind=4), dimension(1:size) :: base
     end subroutine ctcar_regarea_real4_f

     subroutine ctcar_regarea_real8_f(base, size, areaid) bind(c, name='ctcar_regarea_real8_f')
       import
       integer(kind=4) :: areaid
       integer(kind=8) :: size
       real(kind=8), dimension(1:size) :: base
     end subroutine ctcar_regarea_real8_f

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

     subroutine ctcar_sendreq_withint_f(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withint_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withint_f

     subroutine ctcar_sendreq_withreal4_f(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withreal4_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withreal4_f

     subroutine ctcar_sendreq_withreal8_f(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withreal8_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withreal8_f

     subroutine ctcar_sendreq_withint_hdl_f(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withint_hdl_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       integer(kind=4), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withint_hdl_f

     subroutine ctcar_sendreq_withreal4_hdl_f(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withreal4_hdl_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=4), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withreal4_hdl_f

     subroutine ctcar_sendreq_withreal8_hdl_f(intparams, numintparams, data, datanum, hdl) &
          bind(c, name='ctcar_sendreq_withreal8_hdl_f')
       import
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=8), dimension(1:datanum) :: data
       integer(kind=8) :: hdl
     end subroutine ctcar_sendreq_withreal8_hdl_f

     subroutine ctcar_wait(hdl) bind(c, name='ctcar_wait_f')
       import
       integer(kind=8) :: hdl
     end subroutine ctcar_wait

     function ctcar_test_f(hdl) bind(c, name='ctcar_test_f')
       import
       integer(kind=8) :: hdl
       integer(kind=4) :: ctcar_test_f
     end function ctcar_test_f

     subroutine ctcar_finalize() bind(c, name='ctcar_finalize_f')
       import
     end subroutine ctcar_finalize

     subroutine ctcar_prof_start() bind(c, name='ctcar_prof_start_f')
       import
     end subroutine ctcar_prof_start

     subroutine ctcar_prof_stop() bind(c, name='ctcar_prof_stop_f')
       import
     end subroutine ctcar_prof_stop

     subroutine ctcar_prof_start_total() bind(c, name='ctcar_prof_start_total_f')
       import
     end subroutine ctcar_prof_start_total

     subroutine ctcar_prof_stop_total() bind(c, name='ctcar_prof_stop_total_f')
       import
     end subroutine ctcar_prof_stop_total

     subroutine ctcar_prof_start_calc() bind(c, name='ctcar_prof_start_calc_f')
       import
     end subroutine ctcar_prof_start_calc

     subroutine ctcar_prof_stop_calc() bind(c, name='ctcar_prof_stop_calc_f')
       import
     end subroutine ctcar_prof_stop_calc

     subroutine ctcac_init_f() bind(c, name='ctcac_init_f')
       import
     end subroutine ctcac_init_f

     subroutine ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz, bufslotnum) &
          bind(c, name='ctcac_init_detail_f')
       import
       integer(kind=4) :: numareas, numreqs, intparams, bufslotnum
       integer(kind=8) :: bufslotsz
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

     subroutine ctcac_pollreq_withint_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withint_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withint_f

     subroutine ctcac_pollreq_withreal4_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withreal4_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withreal4_f

     subroutine ctcac_pollreq_withreal8_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withreal8_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withreal8_f

     subroutine ctcac_enqreq(reqinfo, progid, intparam, intparamnum) bind(c, name='ctcac_enqreq_f')
       import
       integer(kind=4) :: progid, intparamnum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
     end subroutine ctcac_enqreq

     subroutine ctcac_enqreq_withint_f(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withint_f')
       import
       integer(kind=4) :: progid, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withint_f

     subroutine ctcac_enqreq_withreal4_f(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withreal4_f')
       import
       integer(kind=4) :: progid, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withreal4_f

     subroutine ctcac_enqreq_withreal8_f(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withreal8_f')
       import
       integer(kind=4) :: progid, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withreal8_f

     subroutine ctcac_readarea_int_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_int_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       integer(kind=4), dimension(1:size) :: dest
     end subroutine ctcac_readarea_int_f

     subroutine ctcac_readarea_real4_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=4), dimension(1:size) :: dest
     end subroutine ctcac_readarea_real4_f

     subroutine ctcac_readarea_real8_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcac_readarea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=8), dimension(1:size) :: dest
     end subroutine ctcac_readarea_real8_f

     subroutine ctcac_writearea_int_f(areaid, reqrank, offset, size, src) bind(c, name='ctcac_writearea_int_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       integer(kind=4), dimension(1:size) :: src
     end subroutine ctcac_writearea_int_f

     subroutine ctcac_writearea_real4_f(areaid, reqrank, offset, size, src) bind(c, name='ctcac_writearea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=4), dimension(1:size) :: src
     end subroutine ctcac_writearea_real4_f

     subroutine ctcac_writearea_real8_f(areaid, reqrank, offset, size, src) bind(c, name='ctcac_writearea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=8), dimension(1:size) :: src
     end subroutine ctcac_writearea_real8_f

     subroutine ctcac_finalize() bind(c, name='ctcac_finalize_f')
       import
     end subroutine ctcac_finalize

     subroutine ctcac_prof_start() bind(c, name='ctcac_prof_start_f')
       import
     end subroutine ctcac_prof_start

     subroutine ctcac_prof_stop() bind(c, name='ctcac_prof_stop_f')
       import
     end subroutine ctcac_prof_stop

     subroutine ctcac_prof_start_calc() bind(c, name='ctcac_prof_start_calc_f')
       import
     end subroutine ctcac_prof_start_calc

     subroutine ctcac_prof_stop_calc() bind(c, name='ctcac_prof_stop_calc_f')
       import
     end subroutine ctcac_prof_stop_calc

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

     subroutine ctcaw_pollreq_withint_f(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withint_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       integer(kind=4), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withint_f

     subroutine ctcaw_pollreq_withreal4_f(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withreal4_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=4), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withreal4_f

     subroutine ctcaw_pollreq_withreal8_f(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withreal8_f')
       import
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withreal8_f

     subroutine ctcaw_complete() bind(c, name='ctcaw_complete_f')
       import
     end subroutine ctcaw_complete

     subroutine ctcaw_readarea_int_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_int_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       integer(kind=4), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_int_f

     subroutine ctcaw_readarea_real4_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=4), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_real4_f

     subroutine ctcaw_readarea_real8_f(areaid, reqrank, offset, size, dest) bind(c, name='ctcaw_readarea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=8), dimension(1:size) :: dest
     end subroutine ctcaw_readarea_real8_f

     subroutine ctcaw_writearea_int_f(areaid, reqrank, offset, size, src) bind(c, name='ctcaw_writearea_int_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       integer(kind=4), dimension(1:size) :: src
     end subroutine ctcaw_writearea_int_f

     subroutine ctcaw_writearea_real4_f(areaid, reqrank, offset, size, src) bind(c, name='ctcaw_writearea_real4_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=4), dimension(1:size) :: src
     end subroutine ctcaw_writearea_real4_f

     subroutine ctcaw_writearea_real8_f(areaid, reqrank, offset, size, src) bind(c, name='ctcaw_writearea_real8_f')
       import
       integer(kind=4) :: areaid, reqrank
       integer(kind=8) :: offset, size
       real(kind=8), dimension(1:size) :: src
     end subroutine ctcaw_writearea_real8_f

     subroutine ctcaw_finalize() bind(c, name='ctcaw_finalize_f')
       import
     end subroutine ctcaw_finalize

     subroutine ctcaw_prof_start() bind(c, name='ctcaw_prof_start_f')
       import
     end subroutine ctcaw_prof_start

     subroutine ctcaw_prof_stop() bind(c, name='ctcaw_prof_stop_f')
       import
     end subroutine ctcaw_prof_stop

     subroutine ctcaw_prof_start_calc() bind(c, name='ctcaw_prof_start_calc_f')
       import
     end subroutine ctcaw_prof_start_calc

     subroutine ctcaw_prof_stop_calc() bind(c, name='ctcaw_prof_stop_calc_f')
       import
     end subroutine ctcaw_prof_stop_calc

  end interface

  integer(kind=4) :: CTCA_subcomm, CTCAC_REQINFOITEMS
  integer(kind=4), parameter :: CTCA_ROLE_REQ=0
  integer(kind=4), parameter :: CTCA_ROLE_CPL=1
  integer(kind=4), parameter :: CTCA_ROLE_WRK=2

  public CTCA_subcomm, CTCAC_REQINFOITEMS
  public CTCA_ROLE_REQ, CTCA_ROLE_CPL, CTCA_ROLE_WRK
  public CTCAR_init, CTCAR_init_detail
  public CTCAR_regarea_int,  CTCAR_regarea_real4,  CTCAR_regarea_real8
  public CTCAR_sendreq_withint, CTCAR_sendreq_withreal4, CTCAR_sendreq_withreal8
  public CTCAR_sendreq_withint_hdl, CTCAR_sendreq_withreal4_hdl, CTCAR_sendreq_withreal8_hdl
  public CTCAR_test
  public CTCAC_init, CTCAC_init_detail, CTCAC_isfin
  public CTCAC_pollreq_withint, CTCAC_pollreq_withreal4, CTCAC_pollreq_withreal8
  public CTCAC_enqreq_withint, CTCAC_enqreq_withreal4, CTCAC_enqreq_withreal8
  public CTCAC_readarea_int,  CTCAC_readarea_real4,  CTCAC_readarea_real8
  public CTCAW_init, CTCAW_init_detail, CTCAW_isfin
  public CTCAW_pollreq_withint, CTCAW_pollreq_withreal4, CTCAW_pollreq_withreal8
  public CTCAW_readarea_int,  CTCAW_readarea_real4,  CTCAW_readarea_real8

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

  subroutine ctcar_regarea_int(base, size, areaid) 
    implicit none
    integer(kind=4) :: areaid
    class(*) :: size
    integer(kind=4), dimension(*) :: base
    integer(kind=8) :: size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
          call ctcar_regarea_int_f(base, size64, areaid) 
       type is (integer(kind=8))
          call ctcar_regarea_int_f(base, size, areaid) 
       class default
          print *, "Error : ctcar_regqrea_int : wrong data type for size"
    end select
  end subroutine ctcar_regarea_int
  
  subroutine ctcar_regarea_real4(base, size, areaid) 
    implicit none
    integer(kind=4) :: areaid
    class(*) :: size
    real(kind=4), dimension(*) :: base
    integer(kind=8) :: size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
          call ctcar_regarea_real4_f(base, size64, areaid) 
       type is (integer(kind=8))
          call ctcar_regarea_real4_f(base, size, areaid) 
       class default
          print *, "Error : ctcar_regqrea_real4 : wrong data type for size"
    end select
  end subroutine ctcar_regarea_real4

  subroutine ctcar_regarea_real8(base, size, areaid) 
    implicit none
    integer(kind=4) :: areaid
    class(*) :: size
    real(kind=8), dimension(*) :: base
    integer(kind=8) :: size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
          call ctcar_regarea_real8_f(base, size64, areaid) 
       type is (integer(kind=8))
          call ctcar_regarea_real8_f(base, size, areaid) 
       class default
          print *, "Error : ctcar_regqrea_int : wrong data type for size"
    end select
  end subroutine ctcar_regarea_real8

  subroutine ctcar_sendreq_withint(intparams, numintparams, data, datanum) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    integer(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withint_f(intparams, numintparams, data, datanum64)
       type is (integer(kind=8))
          call ctcar_sendreq_withint_f(intparams, numintparams, data, datanum)
       class default
          print *, "Error : ctcar_sendreq_withint : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withint

  subroutine ctcar_sendreq_withreal4(intparams, numintparams, data, datanum) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    real(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withreal4_f(intparams, numintparams, data, datanum64)
       type is (integer(kind=8))
          call ctcar_sendreq_withreal4_f(intparams, numintparams, data, datanum)
       class default
          print *, "Error : ctcar_sendreq_withreal4 : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withreal4

  subroutine ctcar_sendreq_withreal8(intparams, numintparams, data, datanum) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    real(kind=8), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withreal8_f(intparams, numintparams, data, datanum64)
       type is (integer(kind=8))
          call ctcar_sendreq_withreal8_f(intparams, numintparams, data, datanum)
       class default
          print *, "Error : ctcar_sendreq_withreal8 : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withreal8

  subroutine ctcar_sendreq_withint_hdl(intparams, numintparams, data, datanum, hdl) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    integer(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64
    integer(kind=8) :: hdl

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withint_hdl_f(intparams, numintparams, data, datanum64, hdl)
       type is (integer(kind=8))
          call ctcar_sendreq_withint_hdl_f(intparams, numintparams, data, datanum, hdl)
       class default
          print *, "Error : ctcar_sendreq_withint_hdl : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withint_hdl

  subroutine ctcar_sendreq_withreal4_hdl(intparams, numintparams, data, datanum, hdl) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    real(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64
    integer(kind=8) :: hdl

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withreal4_hdl_f(intparams, numintparams, data, datanum64, hdl)
       type is (integer(kind=8))
          call ctcar_sendreq_withreal4_hdl_f(intparams, numintparams, data, datanum, hdl)
       class default
          print *, "Error : ctcar_sendreq_withreal4_hdl : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withreal4_hdl

  subroutine ctcar_sendreq_withreal8_hdl(intparams, numintparams, data, datanum, hdl) 
    implicit none
    integer(kind=4) :: numintparams
    class(*) :: datanum
    integer(kind=4), dimension(1:numintparams) :: intparams
    real(kind=8), dimension(*) :: data
    integer(kind=8) :: datanum64
    integer(kind=8) :: hdl

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcar_sendreq_withreal8_hdl_f(intparams, numintparams, data, datanum64, hdl)
       type is (integer(kind=8))
          call ctcar_sendreq_withreal8_hdl_f(intparams, numintparams, data, datanum, hdl)
       class default
          print *, "Error : ctcar_sendreq_withreal8_hdl : wrong data type for datanum"
    end select

  end subroutine ctcar_sendreq_withreal8_hdl

  subroutine ctcac_init()
    implicit none
    call ctcac_init_f()
    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcac_init

  subroutine ctcac_init_detail(numareas, numreqs, intparams, bufslotsz, bufslotnum) 
    implicit none
    integer(kind=4) :: numareas, numreqs, intparams, bufslotnum
    class(*) :: bufslotsz
    integer(kind=8) :: bufslotsz64

    select type (bufslotsz)
       type is (integer(kind=4))
          bufslotsz64 = bufslotsz
          call ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz64, bufslotnum)
       type is (integer(kind=8))
          call ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz, bufslotnum)
       class default
          print *, "Error : ctcac_init_detail : wrong data type for bufslotsz"
    end select

    CTCA_subcomm = ctca_get_subcomm()
    CTCAC_REQINFOITEMS = ctca_get_reqinfoitems()
  end subroutine ctcac_init_detail

  function ctcar_test(hdl)
    implicit none
    integer(kind=8) :: hdl
    logical :: ctcar_test

    if (ctcar_test_f(hdl) == 1) then
       ctcar_test = .true.
    else
       ctcar_test = .false.
    end if
  end function ctcar_test

  subroutine ctcac_pollreq_withint(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    integer(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_pollreq_withint_f(reqinfo, fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_pollreq_withint_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withint : wrong data type for datanum"
    end select

  end subroutine ctcac_pollreq_withint

  subroutine ctcac_pollreq_withreal4(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_pollreq_withreal4_f(reqinfo, fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_pollreq_withreal4_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal4 : wrong data type for datanum"
    end select

  end subroutine ctcac_pollreq_withreal4

  subroutine ctcac_pollreq_withreal8(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=8), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_pollreq_withreal8_f(reqinfo, fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_pollreq_withreal8_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal8 : wrong data type for datanum"
    end select

  end subroutine ctcac_pollreq_withreal8

  subroutine ctcac_enqreq_withint(reqinfo, progid, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: progid, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    integer(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_enqreq_withint_f(reqinfo, progid, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_enqreq_withint_f(reqinfo, progid, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withint : wrong data type for datanum"
    end select
  end subroutine ctcac_enqreq_withint

  subroutine ctcac_enqreq_withreal4(reqinfo, progid, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: progid, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_enqreq_withreal4_f(reqinfo, progid, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_enqreq_withreal4_f(reqinfo, progid, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal4 : wrong data type for datanum"
    end select
  end subroutine ctcac_enqreq_withreal4

  subroutine ctcac_enqreq_withreal8(reqinfo, progid, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: progid, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:4) :: reqinfo
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=8), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcac_enqreq_withreal8_f(reqinfo, progid, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcac_enqreq_withreal8_f(reqinfo, progid, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal8 : wrong data type for datanum"
    end select
  end subroutine ctcac_enqreq_withreal8

  subroutine ctcac_readarea_int(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    integer(kind=4), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_readarea_int : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_readarea_int : wrong data type for offset"
    end select

    call ctcac_readarea_int_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcac_readarea_int

  subroutine ctcac_readarea_real4(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=4), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_readarea_real4 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_readarea_real4 : wrong data type for offset"
    end select

    call ctcac_readarea_real4_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcac_readarea_real4

  subroutine ctcac_readarea_real8(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=8), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_readarea_real8 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_readarea_real8 : wrong data type for offset"
    end select

    call ctcac_readarea_real8_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcac_readarea_real8

  subroutine ctcac_writearea_int(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    integer(kind=4), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_writearea_int : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_writearea_int : wrong data type for offset"
    end select

    call ctcac_writearea_int_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcac_writearea_int

  subroutine ctcac_writearea_real4(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=4), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_writearea_real4 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_writearea_real4 : wrong data type for offset"
    end select

    call ctcac_writearea_real4_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcac_writearea_real4

  subroutine ctcac_writearea_real8(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=8), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcac_writearea_real8 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcac_writearea_real8 : wrong data type for offset"
    end select

    call ctcac_writearea_real8_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcac_writearea_real8

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

  subroutine ctcaw_pollreq_withint(fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:intparamnum) :: intparam
    integer(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcaw_pollreq_withint_f(fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcaw_pollreq_withint_f(fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withint : wrong data type for datanum"
    end select
  end subroutine ctcaw_pollreq_withint

  subroutine ctcaw_pollreq_withreal4(fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=4), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcaw_pollreq_withreal4_f(fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcaw_pollreq_withreal4_f(fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal4 : wrong data type for datanum"
    end select
  end subroutine ctcaw_pollreq_withreal4

  subroutine ctcaw_pollreq_withreal8(fromrank, intparam, intparamnum, data, datanum) 
    implicit none
    integer(kind=4) :: fromrank, intparamnum
    class(*) :: datanum
    integer(kind=4), dimension(1:intparamnum) :: intparam
    real(kind=8), dimension(*) :: data
    integer(kind=8) :: datanum64

    select type (datanum)
       type is (integer(kind=4))
          datanum64 = datanum
          call ctcaw_pollreq_withreal8_f(fromrank, intparam, intparamnum, data, datanum64) 
       type is (integer(kind=8))
          call ctcaw_pollreq_withreal8_f(fromrank, intparam, intparamnum, data, datanum) 
       class default
          print *, "Error : ctcac_pollreq_withreal8 : wrong data type for datanum"
    end select
  end subroutine ctcaw_pollreq_withreal8

  subroutine ctcaw_readarea_int(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    integer(kind=4), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_readarea_int : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_readarea_int : wrong data type for offset"
    end select

    call ctcaw_readarea_int_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcaw_readarea_int

  subroutine ctcaw_readarea_real4(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=4), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_readarea_real4 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_readarea_real4 : wrong data type for offset"
    end select

    call ctcaw_readarea_real4_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcaw_readarea_real4

  subroutine ctcaw_readarea_real8(areaid, reqrank, offset, size, dest) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=8), dimension(*) :: dest
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_readarea_real8 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_readarea_real8 : wrong data type for offset"
    end select

    call ctcaw_readarea_real8_f(areaid, reqrank, offset64, size64, dest) 

  end subroutine ctcaw_readarea_real8

  subroutine ctcaw_writearea_int(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    integer(kind=4), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_writearea_int : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_writearea_int : wrong data type for offset"
    end select

    call ctcaw_writearea_int_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcaw_writearea_int

  subroutine ctcaw_writearea_real4(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=4), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_writearea_real4 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_writearea_real4 : wrong data type for offset"
    end select

    call ctcaw_writearea_real4_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcaw_writearea_real4

  subroutine ctcaw_writearea_real8(areaid, reqrank, offset, size, src) 
    implicit none
    integer(kind=4) :: areaid, reqrank
    class(*) :: offset, size
    real(kind=8), dimension(*) :: src
    integer(kind=8) :: offset64, size64

    select type (size)
       type is (integer(kind=4))
          size64 = size
       type is (integer(kind=8))
          size64 = size
       class default
          print *, "Error : ctcaw_writearea_real8 : wrong data type for size"
    end select

    select type (offset)
       type is (integer(kind=4))
          offset64 = offset
       type is (integer(kind=8))
          offset64 = offset
       class default
          print *, "Error : ctcaw_writearea_real8 : wrong data type for offset"
    end select

    call ctcaw_writearea_real8_f(areaid, reqrank, offset64, size64, src) 

  end subroutine ctcaw_writearea_real8

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

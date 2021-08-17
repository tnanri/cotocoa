module ctca_int8
  use, intrinsic :: iso_c_binding
  implicit none
  public

  interface
     subroutine ctcar_sendreq_withreal8_f(intparams, numintparams, data, datanum) &
          bind(c, name='ctcar_sendreq_withreal8_f')
       integer(kind=4) :: numintparams
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:numintparams) :: intparams
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcar_sendreq_withreal8_f

     subroutine ctcac_init_detail_f(numareas, numreqs, intparams, bufslotsz, bufslotnum) &
          bind(c, name='ctcac_init_detail_f')
       integer(kind=4) :: numareas, numreqs, intparams, bufslotnum
       integer(kind=8) :: bufslotsz
     end subroutine ctcac_init_detail_f

     subroutine ctcac_pollreq_withreal8_f(reqinfo, fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_pollreq_withreal8_f')
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_pollreq_withreal8_f
     
     subroutine ctcac_enqreq_withreal8_f(reqinfo, progid, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcac_enqreq_withreal8_f')
       integer(kind=4) :: progid, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:4) :: reqinfo
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcac_enqreq_withreal8_f

     subroutine ctcaw_pollreq_withreal8_f(fromrank, intparam, intparamnum, data, datanum) &
          bind(c, name='ctcaw_pollreq_withreal8_f')
       integer(kind=4) :: fromrank, intparamnum
       integer(kind=8) :: datanum
       integer(kind=4), dimension(1:intparamnum) :: intparam
       real(kind=8), dimension(1:datanum) :: data
     end subroutine ctcaw_pollreq_withreal8_f
  end interface

end module

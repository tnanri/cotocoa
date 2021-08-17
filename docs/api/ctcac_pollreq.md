CTCAC_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum)
=====

Description
-----

Wait for a request from one of the requester processes.
It returns when a request has arrived or all of the request processes have called 
the routine CTCAR_finalize.

Arguments
-----

- reqinfo

  An array of integer (4byte) with CTCAC_REQINFOITEMS elements. 
  Used to store the information of the received request.
  In case if all of the request processes has called the routine CTCAR_finalize,
  the values of this array should be ignored.

- fromrank

  A pointer to an integer (4byte) variable (C) or an integer (4byte) variable (Fortran). 
  Used to store the rank of the requester process that has sent the request.

- intparams

  An array of integer (4byte). 
  Used to store the integer array included in the request.

- intparamnum

  A value of integer (4byte). 
  The number of elements of the included array.


Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

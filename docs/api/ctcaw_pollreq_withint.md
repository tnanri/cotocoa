CTCAW_pollreq_withint(fromrank, intparam, intparamnum, data, datanum)
=====

Description
-----

Wait for a request with an integer (4byte) array attached from one of the requester processes.
If a valid request has arrived, the parameters and attached data are stored as
specified by the arguments.
If the coupler has called CTCAC_finalize and the request queue is empty, it returns
without retrieving parameters or attached data, and let the succeeding call for
CTCAW_isfin to return 1.

Arguments
-----

- fromrank

  A pointer to an integer (4byte) variable (C) or an integer (4byte) variable (Fortran). 
  Used to store the rank of the requester process that has sent the request.

- intparams

  An array of integer (4byte). 
  Used to store the integer array included in the request.

- intparamnum

  A value of integer (4byte). 
  The number of elements of the included array.

- data

  An array of integer (4byte).
  Used to store the array attached with the request.

- datanum

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements of the attached array.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Worker API](../API-worker.md "Back to CoToCoA Worker API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

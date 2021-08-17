CTCAC_enqreq_withint(reqinfo, progid, intparams, intparamnum, data, datanum)
=====

Description
-----

Enqueue the specified request to the workers that executes the specified program ID with
an integer array to be attached.

Arguments
-----

- reqinfo

  An array of integer (4byte) with CTCAC_REQINFOITEMS elements. 
  The information of the request to be enqueued. 
  This must be retrieved by a preceding routine of polling request (CTCAC_pollreq\*).

- progid

  A value of integer (4byte). 
  The ID of the worker program to be executed on this request.

- intparams

  An array of integer (4byte) to be included in the request.

- intparamnum

  A value of integer (4byte). 
  The number of elements of the included array.

- data

  An array of integer (4byte) to be attached to the request.

- datanum

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements of the attached array.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

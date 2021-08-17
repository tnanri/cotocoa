CTCAR_sendreq_withreal4(intparams, numintparams, data, datanum)
=====

Send a request to the coupler with a real (4byte) array to be attached.

Description
-----

- intparams

  An array of integer (4byte) to be included in the request.

- numintparams

  A value of integer (4byte). 
  The number of elements of the included array.

- data

  An array of real (4byte) to be attached to the request.

- datanum

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements of the attached array.

Return value
-----

(C only) Integer (4byte). 0 means success.


##

[Back to CoToCoA Requester API](../API-requester.md "Back to CoToCoA Requester API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

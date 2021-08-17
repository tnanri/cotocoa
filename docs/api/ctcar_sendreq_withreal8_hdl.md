CTCAR_sendreq_withreal8_hdl(intparams, numintparams, data, datanum, hdl)
=====

Description
-----

Send a request to the coupler with a real (8byte) array to be attached.
Retrieves a handle of the request so that its completion can be tested or waited later.

Arguments
-----

- intparams

  An array of integer (4byte) to be included in the request.

- numintparams

  A value of integer (4byte). 
  The number of elements of the included array.

- data

  An array of real (8byte) to be attached to the request.

- datanum

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements of the attached array.

- hdl

  An integer (8byte) variable (Fortran) or a pointer to an integer (8byte) variable (C). 
  Used to store the handle of the request.
  This handle is used by the routines CTCAR_test or CTCAR_wait.

Return value
-----

(C only) Integer (4byte). 0 means success.


##

[Back to CoToCoA Requester API](../API-requester.md "Back to CoToCoA Requester API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

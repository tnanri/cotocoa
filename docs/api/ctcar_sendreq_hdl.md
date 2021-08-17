CTCAR_sendreq_hdl(intparams, numintparams, hdl)
=====

Send a request to the coupler.
Retrieves a handle of the request so that its completion can be tested or waited later.

Description
-----

- intparams

  An array of integer (4byte) to be included in the request.

- numintparams

  A value of integer (4byte). The number of elements of the included array.

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

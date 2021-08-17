CTCAR_init_detail(numareas, numreqs, intparams)
=====

Description
-----

Initialize the process as one of the requester processes with specified settings. 
CoToCoA assumes that this routine (or CTCAR_init) is invoked only from the 
processes that run the requester program. 

Arguments
-----

- numareas

  A value of integer (4byte). 
  The maximum number of areas to expose.

- numreqs

  A value of integer (4byte). 
  The maximum number of outstanding requests.

- intparams

  A value of integer (4byte). 
  The maximum number of elements of the integer array included in a request.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Requester API](../API-requester.md "Back to CoToCoA Requester API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

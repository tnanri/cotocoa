CTCAC_init_detail(int numareas, int numreqs, int intparams, size_t bufslotsz, int bufslotnum)
=====

Description
-----

Initialize the process as the coupler process with specified settings. 
CoToCoA assumes that this routine (or CTCAC_init) is invoked only from 
the process that runs the coupler program. 

Arguments
-----

- numareas

  A value of integer (4byte). 
  The maximum number of areas to expose.

- numreqs

  A value of integer (4byte). 
  The size of the queue to keep requests for the workers.

- intparams

  A value of integer (4byte). 
  The maximum number of elements of the integer array included in a request.

- bufslotsz

  A value of integer (8byte). 
  The maximum size (in byte) of the data to be attached.

- bufslotnum

  A value of integer (4byte). 
  The maximum number of slots prepared to keep the data to be attached.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

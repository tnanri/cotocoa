CTCAR_init()
=====

Description
-----

Initialize the process as one of the requester processes with default settings.
CoToCoA assumes that this routine (or CTCAR_init_detail) is invoked only from 
the processes that run the requester program. The default settings used in this
routine to set up the internal structure are as follows:

- Maximum number of areas to expose = 10
- Maximum number of outstanding requests = 10
- Maximum number of elements of the integer array included in a request = 10

Arguments
-----

None.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Requester API](../API-requester.md "Back to CoToCoA Requester API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

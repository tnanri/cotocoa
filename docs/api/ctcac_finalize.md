CTCAC_finalize()
=====

Description
-----

Finalize the process. 
After calling this routine, the worker routine CTCAW_isfin begins to return 1
if the request queue has become empty.

This routine waits for finalization of all processes of the requester, the coupler
and the workers.

Arguments
-----

None.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

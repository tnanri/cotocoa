CTCAR_finalize()
=====

Description
-----

Finalize the process. After all processes of the requester call this routine,
the coupler routine CTCAC_isfin begins to return 1.

This routine waits for finalization of all processes of the requester, the coupler
and the workers.

If the routine CTCAR_prof_start has been invoked at least once, this routine
prints out the result of the profiling.


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

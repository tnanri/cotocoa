CTCAR_prof_start()
=====

Description
-----

Starts profiling time consumed by CoToCoA routines. This routine can be invoked again after calling
CTCAR_prof_stop so that some specific regions of the program are profiled.


If this routine has been invoked at least once, the routine CTCAR_finalize prints out the result of 
the profiling.

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

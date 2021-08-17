CTCAW_complete()
=====

Description
-----

Waits for all processes in this subcommunicator to call this routine.
Then, it notifies the coupler that the subcommunicator is now idle 
and ready for another request.
In addition to that, if the completed request was sent by the requester 
with handle to wait, this routine also notifies the requester that it 
has been completed.

Arguments
-----

None.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Worker API](../API-worker.md "Back to CoToCoA Worker API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

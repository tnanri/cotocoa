CTCAW_isfin()
=====

Description
-----

Checks if the coupler has called CTCAC_finalize and the request queue is empty.
This routine must be called immediately after polling for a request (by CTCAW_pollreq*).

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

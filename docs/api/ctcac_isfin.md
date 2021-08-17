CTCAC_isfin()
=====

Description
-----

Checks if all processes of the requester have called CTCAR_finalize.
This routine must be called immediately after polling for a request (by CTCAC_pollreq*).

Arguments
-----

None.

Return value
-----

Integer (4byte). 
0 means some processes of the requester have not called CTCAR_finalize.
In this case, the preceding call for polling a request has retrieved a valid information
of the request.
On the other hand, 1 means all processes of the requester have called CTCAR_finalize.
In this case, the coupler may call CTCAC_finalize to notify the workers 
that the requester and the coupler are ready to quit from CoToCoA.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

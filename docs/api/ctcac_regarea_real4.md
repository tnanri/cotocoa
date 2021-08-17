CTCAC_regarea_real4(areaid)
=====

Description
-----

Participate to a registration of a real (4byte) array of the requester.
This routine is collective, that means,
this routine returns after all processes of the requester, the coupler
and the workers have called corresponding routine of area registration.

At each invocation of area registration, a unique ID is attached to the area.

Arguments
-----

- areaid

  An integer (4byte) variable (Fortran) or a pointer to an integer (4byte) variable (C).
  Used to store the area ID.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Coupler API](../API-coupler.md "Back to CoToCoA Coupler API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

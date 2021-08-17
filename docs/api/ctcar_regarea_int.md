CTCAR_regarea_int(base, size, areaid)
=====

Description
-----

Register an integer (4byte) array as an area to be exposed so that
the coupler and the workers can read or write.
This routine is collective, that means,
this routine returns after all processes of the requester have called this routine,
as well as all processes of the coupler and the workers have called corresponding
routine of area registration.

At each invocation of area registration, a unique ID is attached to the area.
If multiple processes of the requester have passed non-zero value as the array to be registerred,
the same area ID is attached to all of those arrays.
In that case, the exact array to be read or written can be specified by 
the area ID and the rank of the process within the requester.

Arguments
-----

- base

  An array of integer (4byte) to be registerred as an area.

- size

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements of the array to be registerred.

- areaid

  An integer (4byte) variable (Fortran) or a pointer to an integer (4byte) variable (C). 
  Used to store the area ID.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Requester API](../API-requester.md "Back to CoToCoA Requester API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

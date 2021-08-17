CTCAW_writearea_int(areaid, reqrank, offset, size, src)
=====

Description
-----

Write to the integer (4byte) array in the requester exposed as an area. 
It returns after the write has been completed.

Arguments
-----

- areaid

  A value of integer (4byte). 
  The ID of the target area to write. 
  This must be retrieved by a preceding call of CTCAW_regarea_real8.

- reqrank

  A value of integer (4byte). 
  The rank of the target process among the requester processes to write. 
  This must be within the range of ranks of the subcommunicator of the requester.

- offset

  A value of size_t (C) or integer (8byte) (Fortran). 
  The offset index from the beginning of the area from where to write.

- size

  A value of size_t (C) or integer (8byte) (Fortran). 
  The number of elements to write.

- src

  An array of integer (4byte).
  The start position of the original array of the write.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Worker API](../API-worker.md "Back to CoToCoA Worker API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")


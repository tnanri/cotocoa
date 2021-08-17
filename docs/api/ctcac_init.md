CTCAC_init()
=====

Description
-----

Initialize the process the coupler process with default settings.
CoToCoA assumes that this routine (or CTCAC_init_detail) is invoked only from 
the process that runs the coupler program. The default settings used in this
routine to set up the internal structure are as follows:

- Maximum number of areas to expose = 10
- Size of the queue to keep requests for the workers = 10
- Maximum number of elements of the integer array included in a request = 10
- Maximum size (in byte) of the data to be attached = 80000
- Maximum number of slots prepared to keep the data to be attached = 10

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

CTCAW_init(progid, procspercomm)
=====

Description
-----

Initialize the process as one of the processes of the workers.
CoToCoA assumes that this routine (or CTCAW_init_detail) is invoked only from 
the process that runs one of the worker program. The default settings used 
in this routine to set up the internal structure are as follows:

- Maximum number of areas to expose = 10
- Maximum number of elements of the integer array included in a request = 10

Arguments
-----

- progid

  A value of integer (4byte). 
  ID of the worker program.
  This ID is used to specify the program to be used when the coupler enqueues
  a request.
  Therefore, CoToCoA assumes that the same value is specified for the same
  worker program.

- procspercomm

  A value of integer (4byte).
  The number of processes per subcommunicator.
  The processes that run the same worker program is divided into one or more 
  subcommunicators with this number of processes.

Return value
-----

(C only) Integer (4byte). 0 means success.

##

[Back to CoToCoA Worker API](../API-worker.md "Back to CoToCoA Worker API")

##

[Back to CoToCoA API](../API.md "Back to CoToCoA API")

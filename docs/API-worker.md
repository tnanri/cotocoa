CoToCoA Worker API
=====

## Initialize
- [CTCAW_init](api/ctcaw_init.md "CTCAW_init")

    Initialize CoToCoA as one of the processes of the worker.

- [CTCAW_init_detail](api/ctcaw_init_detail.md "CTCAW_init_detail")

    Initialize CoToCoA as one of the processes of the worker with detailed settings.

## Finalize
- [CTCAW_finalize](api/ctcaw_finalize.md "CTCAW_finalize")

    Finalize CoToCoA.

## Check the finalization of the requester and the coupler
- [CTCAW_isfin](api/ctcaw_isfin.md "CTCAW_isfin")

    Check if all of the processes of the requester has called CTCAR_finalize and there is no remaining requests to execute.    

## Poll request from the coupler
- [CTCAW_pollreq](api/ctcaw_pollreq.md "CTCAW_pollreq")

    Poll for a request from one of the process of the coupler.

- [CTCAW_pollreq_withint](api/ctcaw_pollreq_withint.md "CTCAW_pollreq_withint")

    Poll for a request with an integer array from one of the process of the coupler.

- [CTCAW_pollreq_withreal4](api/ctcaw_pollreq_withreal4.md "CTCAW_pollreq_withreal4")

    Poll for a request with a real (4byte) array from one of the process of the coupler.

- [CTCAW_pollreq_withreal8](api/ctcaw_pollreq_withreal8.md "CTCAW_pollreq_withreal8")

    Poll for a request with a real (8byte) array from one of the process of the coupler.

## Notify completion of the request
- [CTCAW_complete](api/ctcaw_complete.md "CTCAW_complete")

    Notify the coupler (and, if necessary, the requester) that the reuest has been completed and this worker group has become idle.

## Participate registration of the area of the requester
- [CTCAW_regarea_int](api/ctcaw_regarea_int.md "CTCAW_regarea_int")

    Participate the registration of an integer array of the requester to retrieve the area ID.

- [CTCAW_regarea_real4](api/ctcaw_regarea_real4.md "CTCAW_regarea_real4")

    Participate the registration of a real (4byte) array of the requester to retrieve the area ID.

- [CTCAW_regarea_real8](api/ctcaw_regarea_real8.md "CTCAW_regarea_real8")

    Participate the registration of a real (8byte) array of the requester to retrieve the area ID.

## Read data from the area of the requester 
- [CTCAW_readarea_int](api/ctcaw_readarea_int.md "CTCAW_readarea_int")

    Read data from an integer array area of the requester.

- [CTCAW_readarea_real4](api/ctcaw_readarea_real4.md "CTCAW_readarea_real4")

    Read data from a real (4byte) array area of the requester.

- [CTCAW_readarea_real8](api/ctcaw_readarea_real8.md "CTCAW_readarea_real8")

    Read data from a real (8byte) array area of the requester.

## Write data to the area of the requester
- [CTCAW_writearea_int](api/ctcaw_writearea_int.md "CTCAW_writearea_int")

    Write data to an integer array area of the requester.

- [CTCAW_writearea_real4](api/ctcaw_writearea_real4.md "CTCAW_writearea_real4")

    Write data to a real (4byte) array area of the requester.

- [CTCAW_writearea_real8](api/ctcaw_writearea_real8.md "CTCAW_writearea_real8")

    Write data to a real (8byte) array area of the requester.

## Profile
- [CTCAW_prof_start](api/ctcaw_prof_start.md "CTCAW_prof_start")

    Start profiling.    

- [CTCAW_prof_stop](api/ctcaw_prof_stop.md "CTCAW_prof_stop")

    Stop profiling.

- [CTCAW_prof_start_calc](api/ctcaw_prof_start_calc.md "CTCAW_prof_start_calc")

    Start position of the region of "Calculation Time".

- [CTCAW_prof_stop_calc](api/ctcaw_prof_stop_calc.md "CTCAW_prof_stop_calc")

    Stop position of the region of "Calculation Time".

##

[Back to CoToCoA API](API.md "Back to CoToCoA API")

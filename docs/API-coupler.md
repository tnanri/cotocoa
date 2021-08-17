CoToCoA Coupler API
=====

## Initialize
- [CTCAC_init](api/ctcac_init.md "CTCAC_init")

    Initialize CoToCoA as the coupler process.

- [CTCAC_init_detail](api/ctcac_init_detail.md "CTCAC_init_detail")

    Initialize CoToCoA as the coupler process with detailed settings.

## Finalize
- [CTCAC_finalize](api/ctcac_finalize.md "CTCAC_finalize")

    Finalize CoToCoA.

## Check the finalization of the requester
- [CTCAC_isfin](api/ctcac_isfin.md "CTCAC_isfin")

    Check if all of the processes of the requester has called CTCAR_finalize.

## Poll for a request from the requester
- [CTCAC_pollreq](api/ctcac_pollreq.md "CTCAC_pollreq")

    Poll for a request from one of the process of the requester.

- [CTCAC_pollreq_withint](api/ctcac_pollreq_withint.md "CTCAC_pollreq_withint")

    Poll for a request with an integer array from one of the process of the requester.

- [CTCAC_pollreq_withreal4](api/ctcac_pollreq_withreal4.md "CTCAC_pollreq_withreal4")

    Poll for a request with a real (4byte) array from one of the process of the requester.

- [CTCAC_pollreq_withreal8](api/ctcac_pollreq_withreal8.md "CTCAC_pollreq_withreal8")

    Poll for a request with a real (8byte) array from one of the process of the requester.

## Enqueue a request to the worker
- [CTCAC_enqreq](api/ctcac_enqreq.md "CTCAC_enqreq")

    Enqueue a request to one of the workers.

- [CTCAC_enqreq_withint](api/ctcac_enqreq_withint.md "CTCAC_enqreq_withint")

    Enqueue a request to one of the workers with an integer array attached.

- [CTCAC_enqreq_withreal4](api/ctcac_enqreq_withreal4.md "CTCAC_enqreq_withreal4")

    Enqueue a request to one of the workers with a real (4byte) array attached.

- [CTCAC_enqreq_withreal8](api/ctcac_enqreq_withreal8.md "CTCAC_enqreq_withreal8")

    Enqueue a request to one of the workers with a real (8byte) array attached.

## Participate registration of an area of the requester
- [CTCAC_regarea_int](api/ctcac_regarea_int.md "CTCAC_regarea_int")

    Participate the registration of an integer array of the requester to retrieve the area ID.

- [CTCAC_regarea_real4](api/ctcac_regarea_real4.md "CTCAC_regarea_real4")

    Participate the registration of an real (4byte) array of the requester to retrieve the area ID.

- [CTCAC_regarea_real8](api/ctcac_regarea_real8.md "CTCAC_regarea_real8")

    Participate the registration of an real (8byte) array of the requester to retrieve the area ID.

## Read data from the area of the requester
- [CTCAC_readarea_int](api/ctcac_readarea_int.md "CTCAC_readarea_int")

    Read data from an integer array area of the requester.

- [CTCAC_readarea_real4](api/ctcac_readarea_real4.md "CTCAC_readarea_real4")

    Read data from a real (4byte) array area of the requester.

- [CTCAC_readarea_real8](api/ctcac_readarea_real8.md "CTCAC_readarea_real8")

    Read data from a real (8byte) array area of the requester.

## Write data to the area of the requester
- [CTCAC_writearea_int](api/ctcac_writearea_int.md "CTCAC_writearea_int")

    Write data to an integer array area of the requester.

- [CTCAC_writearea_real4](api/ctcac_writearea_real4.md "CTCAC_writearea_real4")

    Write data to a real (4byte) array area of the requester.

- [CTCAC_writearea_real8](api/ctcac_writearea_real8.md "CTCAC_writearea_real8")

    Write data to a real (8byte) array area of the requester.

## Profile
- [CTCAC_prof_start](api/ctcac_prof_start.md "CTCAC_prof_start")

    Start profiling.    

- [CTCAC_prof_stop](api/ctcac_prof_stop.md "CTCAC_prof_stop")

    Stop profiling.

- [CTCAC_prof_start_calc](api/ctcac_prof_start_calc.md "CTCAC_prof_start_calc")

    Start position of the region of "Calculation Time".

- [CTCAC_prof_stop_calc](api/ctcac_prof_stop_calc.md "CTCAC_prof_stop_calc")

    Stop position of the region of "Calculation Time".

##

[Back to CoToCoA API](API.md "Back to CoToCoA API")

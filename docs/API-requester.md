CoToCoA Requester API
=====

## Initialize
- [CTCAR_init](api/ctcar_init.md "CTCAR_init")

    Initialize CoToCoA as one of the requester process.

- [CTCAR_init_detail](api/ctcar_init_detail.md "CTCAR_init_detail")

    Initialize CoToCoA as one of the requester process with detailed settings.

## Finalize
- [CTCAR_finalize](api/ctcar_finalize.md "CTCAR_finalize")

    Finalize CoToCoA.

## Send request
- [CTCAR_sendreq](api/ctcar_sendreq.md "CTCAR_sendreq")

    Send a request to the coupler.

- [CTCAR_sendreq_hdl](api/ctcar_sendreq_hdl.md "CTCAR_sendreq_hdl")

    Send a request to the coupler and retrieve a handle to wait completion.

- [CTCAR_sendreq_withint](api/ctcar_sendreq_withint.md "CTCAR_sendreq_withint")

    Send a request to the coupler with an integer array attached.

- [CTCAR_sendreq_withreal4](api/ctcar_sendreq_withreal4.md "CTCAR_sendreq_withreal4")

    Send a request to the coupler with a real (4byte) array attached.

- [CTCAR_sendreq_withreal8](api/ctcar_sendreq_withreal8.md "CTCAR_sendreq_withreal8")

    Send a request to the coupler with a real (8byte) array attached.

- [CTCAR_sendreq_withint_hdl](api/ctcar_sendreq_withint_hdl.md "CTCAR_sendreq_withint_hdl")

    Send a request to the coupler with an integer array attached and retrieve a handle to wait completion.

- [CTCAR_sendreq_withreal4_hdl](api/ctcar_sendreq_withreal4_hdl.md "CTCAR_sendreq_withreal4_hdl")

    Send a request to the coupler with a real (4byte) array attached and retrieve a handle to wait completion.

- [CTCAR_sendreq_withreal8_hdl](api/ctcar_sendreq_withreal8_hdl.md "CTCAR_sendreq_withreal8_hdl")

    Send a request to the coupler with a real (8byte) array attached and retrieve a handle to wait completion.

## Wait/Test completion of request
- [CTCAR_wait](api/ctcar_wait.md "CTCAR_wait")

    Wait completion of the request specified by the handle.

- [CTCAR_test](api/ctcar_test.md "CTCAR_test")

    Check if the request specified by the handle has been completed.

## Register area to be read/written
- [CTCAR_regarea_int](api/ctcar_regarea_int.md "CTCAR_regarea_int")

    Register an integer array as an area to be exposed for remote read/write from the coupler or the workers.

- [CTCAR_regarea_real4](api/ctcar_regarea_real4.md "CTCAR_regarea_real4")

    Register a real (4byte) array as an area to be exposed for remote read/write from the coupler or the workers.

- [CTCAR_regarea_real8](api/ctcar_regarea_real8.md "CTCAR_regarea_real8")

    Register a real (8byte) array as an area to be exposed for remote read/write from the coupler or the workers.

## Profile
- [CTCAR_prof_start](api/ctcar_prof_start.md "CTCAR_prof_start")

    Start profiling.

- [CTCAR_prof_stop](api/ctcar_prof_stop.md "CTCAR_prof_stop")

    Stop profiling.

- [CTCAR_prof_start_total](api/ctcar_prof_start_total.md "CTCAR_prof_start_total")

    Start position of the region of "Total Time".

- [CTCAR_prof_stop_total](api/ctcar_prof_stop_total.md "CTCAR_prof_stop_total")

    Stop position of the region of "Total Time".

- [CTCAR_prof_start_calc](api/ctcar_prof_start_calc.md "CTCAR_prof_start_calc")

    Start position of the region of "Calculation Time".

- [CTCAR_prof_stop_calc](api/ctcar_prof_stop_calc.md "CTCAR_prof_stop_calc")

    Stop position of the region of "Calculation Time".

##

[Back to CoToCoA API](API.md "Back to CoToCoA API")

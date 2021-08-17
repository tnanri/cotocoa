Usage of CoToCoA
=====

1. Modify existing programs
-----

CoToCoA assumes the users to couple two (or more) existing MPI programs.

CoToCoA requires at least the following modifications to those programs:
- Replace MPI_Init and MPI_Finalize with the routines of initialization and finalization of CoToCoA.
- Change MPI_COMM_WORLD to CTCA_Subcomm.

According to the type of the program, some additional changes are needed:
- Requester
    - Add routines to send requests.
    - Attache data to each request, or add routines to expose memory areas to be read/written by the coupler or the workers.
- Worker
    - Make a main loop that repeats the following operations: 
        - Deque request from the queue.
        - If the requester has quitted CoToCoA, quit from the main loop.
        - Initialize the work.
        - Read data from the requester (if necessary).
        - Compute with the request.
        - Write-back data to the requester (if necessary).
        - Notify completion of the request.

2. Prepare a coupler program
-----

Usually, the coupler program is newly created or copied from the previous coupled computation.

The simplest design is as follows:
- Initialize CoToCoA
- Main loop:
    - Accept request from the requester
    - If the requester has quitted CoToCoA, quit from the main loop.
    - Enqueue the request to the queue of the target worker.
- Finalize CoToCoA
 
3. Compile all programs
-----

Add the following flags to compile and link CoToCoA library:
- C
    - ``-I${cotocoa_dir}/include``
    - ``-L${cotocoa_dir}/lib -lctca``
- Fortran
    - ``-I${cotocoa_dir}/include``
    - ``-L${cotocoa_dir}/lib -lctca -lctca_f -lctca_mod``

4. Run the programs
-----

Use MPMD style of mpirun to start programs of the requester, the coupler and the workers.

    Example (8 procs for the requester, 1 proc for the coupler and 4 procs for each of the three workers):

    `mpirun  -n 8 ./sample_requester : -n 1 ./sample_coupler : -n 4 ./sample_worker0 : -n 4 ./sample_worker1 : -n 8 ./sample_worker2`


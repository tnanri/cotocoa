#!/bin/bash
#PJM -L "rscunit=ito-a"
#PJM -L "rscgrp=ito-s-dbg"
#PJM -L "vnode=4"
#PJM -L "vnode-core=36"
#PJM -L "elapse=00:20:00"
#PJM -j
#PJM -S

export LANG=C
module load intel/2020.1

export I_MPI_PERHOST=8
export I_MPI_FABRICS=shm:ofi
export I_MPI_PIN_DOMAIN=omp
export I_MPI_PIN_CELL=core

export KMP_STACKSIZE=8m
export KMP_AFFINITY=compact

export I_MPI_HYDRA_BOOTSTRAP=rsh
export I_MPI_HYDRA_BOOTSTRAP_EXEC=/bin/pjrsh
export I_MPI_HYDRA_HOST_FILE=${PJM_O_NODEINF}

mpiexec.hydra -n 4 ./sample2_requester : -n 1 ./sample2_coupler : -n 8 ./sample2_worker1 : -n 8 ./sample2_worker2 : -n 8 ./sample2_worker3










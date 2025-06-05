#!/bin/sh
#PJM -L rscgrp=a-batch
#PJM -L node=2
#PJM -L elapse=00:10:00
#PJM -j
module load intel impi
CTCAPREFIX=${HOME}/local/cotocoa/1.3.1-test
export PYTHONPATH="${CTCAPREFIX}/lib:${PYTHONPATH}"

mpiexec -n 4 ./sample4_requester : -n 1 ./sample4_coupler : -n 2 python3.11 ./sample4_worker.py




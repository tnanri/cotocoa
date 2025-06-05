#!/bin/sh
#PJM -L rscgrp=a-batch
#PJM -L node=2
#PJM -L elapse=00:10:00
#PJM -j

module load intel impi

mpiexec -n 4 ./sample3_requester : -n 1 ./sample3_coupler : -n 8 ./sample3_worker










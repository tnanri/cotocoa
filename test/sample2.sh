#!/bin/sh
#PJM -L rscgrp=a-batch
#PJM -L node=2
#PJM -L elapse=00:10:00
#PJM -j

module load intel impi

mpiexec -n 4 ./sample2_requester : -n 1 ./sample2_coupler : -n 8 ./sample2_worker1 : -n 8 ./sample2_worker2 : -n 8 ./sample2_worker3










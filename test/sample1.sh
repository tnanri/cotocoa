#!/bin/sh
#PJM -L rscgrp=a-batch
#PJM -L node=2
#PJM -L elapse=00:10:00
#PJM -j

module load intel impi

mpiexec -n 4 ./sample1_requester : -n 1 ./sample1_coupler : -n 8 ./sample1_worker1 : -n 8 ./sample1_worker2 : -n 8 ./sample1_worker3









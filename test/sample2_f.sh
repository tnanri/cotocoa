#!/bin/bash
#PJM -L "rscunit=ito-a"
#PJM -L "rscgrp=ito-m-dbg"
#PJM -L "vnode=16"
#PJM -L "vnode-core=36"
#PJM -L "elapse=01:00:00"
#PJM -j
#PJM -S
#PJM -X

export LANG=C
module load openmpi/3.1.3-nocuda-intel18.3
#module load mvapich/2.3-nocuda-intel18.3

mpirun --map-by ppr:2:node --bind-to none -mca plm_rsh_agent /bin/pjrsh -machinefile ${PJM_O_NODEINF} -mca btl_openib_allow_ib true -n 4 ./sample2_requester_f : -n 1 ./sample2_coupler_f : -n 8 ./sample2_worker1_f : -n 8 ./sample2_worker2_f : -n 8 ./sample2_worker3_f
#mpiexec -machinefile ${PJM_O_NODEINF} -launcher ssh -launcher-exec /bin/pjrsh -bind-to rr -n 4 ./sample_requester : -n 1 ./sample_coupler : -n 8 ./sample_worker1 : -n 8 ./sample_worker2 : -n 8 ./sample_worker3
#/home/usr3/k70043a/local/openmpi-4.0.0-noucx/bin/mpirun --map-by ppr:2:node --bind-to none -mca plm_rsh_agent /bin/pjrsh -machinefile ${PJM_O_NODEINF} -mca btl_openib_allow_ib true -n 4 ./sample_requester : -n 1 ./sample_coupler : -n 8 ./sample_worker1 : -n 8 ./sample_worker2 : -n 8 ./sample_worker3










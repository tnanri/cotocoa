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

mpirun --map-by ppr:2:node --bind-to none -mca plm_rsh_agent /bin/pjrsh -machinefile ${PJM_O_NODEINF} -mca btl_openib_allow_ib true -n 4 ./sample1_requester_f : -n 1 ./sample1_coupler_f : -n 8 ./sample1_worker1_f : -n 8 ./sample1_worker2_f : -n 8 ./sample1_worker3_f











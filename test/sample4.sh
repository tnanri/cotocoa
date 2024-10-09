#!/bin/bash
#============ Slurm Options(KyotoU Supercomputer) ===========
#SBATCH -p gr10034b
#SBATCH -t 1:00:00
#SBATCH --rsc p=9:t=1:c=1

#============ Shell Script ============
export PYTHONUNBUFFERED=1
set -x

# srun --multi-prog multi.conf -n 4 ./sample1_requester : -n 1 ./sample1_coupler : -n 8 ./sample1_worker1
srun --multi-prog multi1.conf

# contents of multi1.conf file as below:
# 0-3 ./sample1_requester
# 4 ./sample1_coupler
# 5-8 python3.8 ./sample1_worker4.py
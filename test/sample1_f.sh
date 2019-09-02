#!/bin/bash
mpirun -n 4 ./sample1_requester_f : -n 1 ./sample1_coupler_f : -n 8 ./sample1_worker1_f : -n 8 ./sample1_worker2_f : -n 8 ./sample1_worker3_f











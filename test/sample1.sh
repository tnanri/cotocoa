#!/bin/bash
mpirun -n 4 ./sample1_requester_c : -n 1 ./sample1_coupler_c : -n 8 ./sample1_worker1_c : -n 8 ./sample1_worker2_c : -n 8 ./sample1_worker3_c









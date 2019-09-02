#!/bin/bash

mpirun -n 4 ./sample2_requester_f : -n 1 ./sample2_coupler_f : -n 8 ./sample2_worker1_f : -n 8 ./sample2_worker2_f : -n 8 ./sample2_worker3_f










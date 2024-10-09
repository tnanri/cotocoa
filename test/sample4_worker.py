import ctypes
import os
from mpi4py import MPI
import numpy as np
import sys
# sys.path.append('/home/a/a0238138') # specify the path to CTCAWorker file
from ctca4py import CTCAWorker

# import ctca4py class from ctca4py file
ctca=CTCAWorker()
progid=0
procspercomm=2

print("python worker init")
ctca.CTCAW_init(progid,procspercomm)
print("python worker init done")

ctca.CTCAW_prof_start()
ctca.CTCAW_prof_start_calc()

print("get sub comm")
subrank=ctca.CTCA_subcomm.Get_rank()
print("python worker subrank",subrank)

while True:
    datanum=10000//8
    # data=np.zeros(datanum,dtype=np.float64)
    data=np.zeros(datanum,dtype=np.float64)
    fromrank=0

    intparamsnum=2

    fromrank,intparam,data=ctca.CTCAW_pollreq(intparamsnum,datanum,CTCAWorker.DAT_REAL8)
    
    if ctca.CTCAW_isfin()==1:
        #print("work3 finilize")
        break
    print("worker1 poll req done")

    # ctca.subcomm.Bcast(data,root=0)

    print("worker1 bcast done")

    print("python worker world_rank %d, subrank %d data %f %f %f from requester rank %d"%(ctca.world_rank,subrank,data[0],data[datanum//2], data[datanum-1],fromrank))

    # CTCAW_complete()
    ctca.CTCAW_complete()
    print("python worker complete done")


ctca.CTCAW_prof_stop_calc()
ctca.CTCAW_prof_stop()

ctca.CTCAW_finalize()

print("%d:python worker finalize"%(subrank))
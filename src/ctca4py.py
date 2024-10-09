from mpi4py import MPI
import ctypes
import os
import numpy as np

class CTCAWorker:
    DEF_MAXNUMAREAS = 10
    DEF_MAXINTPARAMS = 10

    # tags of data's type
    DAT_INT=0   # int
    DAT_REAL4=1 # float
    DAT_REAL8=2 # double

    # tags of area's type
    AREA_INT=0
    AREA_REAL4=1
    AREA_REAL8=2
    
    def __init__(self) -> None:

        self.CTCA_subcomm=MPI.COMM_WORLD
        self.world_rank = MPI.COMM_WORLD.Get_rank()
        self.world_nprocs = MPI.COMM_WORLD.Get_size()
        # self.subcomm=0
        self.subrank=0

        # counter
        self.CTCAW_pollreq_counter=0       # number of CTCAW_pollreq calls
        self.CTCAW_readarea_counter=0
        self.CTCAW_writearea_counter=0
        
        # execution time
        self.WRK_pollreq_t=0 # time of CTCAW_pollreq
        self.WRK_readarea_t=0    # time of CTCAW_readarea
        self.CTCAW_writearea_t=0 

        self.areaid=0    #area id

        # load shared library
        self.c_lib=ctypes.CDLL(os.path.abspath('/home/a/a0238138/ctca4py/ctca4py.so'))

        # CTCAW_init
        self.c_lib.CTCAW_init.argtypes=[ctypes.c_int,ctypes.c_int]
        self.c_lib.CTCAW_init.restype=ctypes.c_int

        # CTCAW_init_detail
        self.c_lib.CTCAW_init_detail.argtypes=[ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int]
        self.c_lib.CTCAW_init_detail.restype=ctypes.c_int

        # wrk_pollreq
        self.c_lib.wrk_pollreq.argtypes = [
            ctypes.POINTER(ctypes.c_int),  # int *fromrank
            ctypes.POINTER(ctypes.c_int),  # int *intparam
            ctypes.c_int,                  # int intparamnum
            ctypes.c_void_p,               # void *data
            ctypes.c_size_t                # size_t datasz
        ]
        self.c_lib.wrk_pollreq.restype = ctypes.c_int

        # CTCAW_pollreq
        self.c_lib.CTCAW_pollreq.argtypes=[ctypes.POINTER(ctypes.c_int),\
                                                       ctypes.POINTER(ctypes.c_int),ctypes.c_int]
        self.c_lib.CTCAW_pollreq.restype=ctypes.c_int

        # CTCAW_pollreq_withint
        self.c_lib.CTCAW_pollreq_withint.argtypes=[ctypes.POINTER(ctypes.c_int),\
                                                       ctypes.POINTER(ctypes.c_int),ctypes.c_int,\
                                                        ctypes.POINTER(ctypes.c_int),ctypes.c_size_t]
        self.c_lib.CTCAW_pollreq_withint.restype=ctypes.c_int

        # CTCAW_pollreq_withreal4
        self.c_lib.CTCAW_pollreq_withreal4.argtypes=[ctypes.POINTER(ctypes.c_int),\
                                                       ctypes.POINTER(ctypes.c_int),ctypes.c_int,\
                                                        ctypes.POINTER(ctypes.c_float),ctypes.c_size_t]
        self.c_lib.CTCAW_pollreq_withreal4.restype=ctypes.c_int


        # CTCAW_pollreq_withreal8
        self.c_lib.CTCAW_pollreq_withreal8.argtypes=[ctypes.POINTER(ctypes.c_int),\
                                                       ctypes.POINTER(ctypes.c_int),ctypes.c_int,\
                                                        ctypes.POINTER(ctypes.c_double),ctypes.c_size_t]
        self.c_lib.CTCAW_pollreq_withreal8.restype=ctypes.c_int

        # CTCAW_complete
        self.c_lib.CTCAW_complete.restype=ctypes.c_int

        # CTCAW_isfin
        self.c_lib.CTCAW_isfin.restype=ctypes.c_int

        # CTCAW_finalize
        self.c_lib.CTCAW_finalize.restype=ctypes.c_int

        # CTCA_get_subcomm
        self.c_lib.CTCA_get_subcomm.restype=ctypes.c_int

        # CTCAW_prof_start
        self.c_lib.CTCAW_prof_start.restype=ctypes.c_int
        # CTCAW_prof_stop
        self.c_lib.CTCAW_prof_stop.restype=ctypes.c_int
        # CTCAW_prof_start_calc
        self.c_lib.CTCAW_prof_start_calc.restype=ctypes.c_int
        # CTCAW_prof_stop_calc
        self.c_lib.CTCAW_prof_stop_calc.restype=ctypes.c_int

        # CTCAW_regarea
        self.c_lib.CTCAW_regarea_int.argtypes=[ctypes.POINTER(ctypes.c_int)]
        self.c_lib.CTCAW_regarea_int.restype=ctypes.c_int
        self.c_lib.CTCAW_regarea_real4.argtypes=[ctypes.POINTER(ctypes.c_int)]
        self.c_lib.CTCAW_regarea_real4.restype=ctypes.c_int
        self.c_lib.CTCAW_regarea_real8.argtypes=[ctypes.POINTER(ctypes.c_int)]
        self.c_lib.CTCAW_regarea_real8.restype=ctypes.c_int

        # CTCAW_readarea
        self.c_lib.CTCAW_readarea_int.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_int)]
        self.c_lib.CTCAW_readarea_int.restype=ctypes.c_int

        self.c_lib.CTCAW_readarea_real4.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_float)]
        self.c_lib.CTCAW_readarea_real4.restype=ctypes.c_int

        self.c_lib.CTCAW_readarea_real8.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_double)]
        self.c_lib.CTCAW_readarea_real8.restype=ctypes.c_int

        # CTCAW_writearea
        self.c_lib.CTCAW_writearea_int.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_int)]
        self.c_lib.CTCAW_writearea_int.restype=ctypes.c_int

        self.c_lib.CTCAW_writearea_real4.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_float)]
        self.c_lib.CTCAW_writearea_real4.restype=ctypes.c_int

        self.c_lib.CTCAW_writearea_real8.argtypes=[ctypes.c_int,ctypes.c_int,\
                                                    ctypes.c_size_t,ctypes.c_size_t,\
                                                    ctypes.POINTER(ctypes.c_double)]
        self.c_lib.CTCAW_writearea_real8.restype=ctypes.c_int



    def CTCA_get_subcomm(self):
        comm_c=self.c_lib.CTCA_get_subcomm()
        self.CTCA_subcomm=MPI.Comm.f2py(comm_c)
    
    def CTCAW_init(self,progid,procspercomm)->None:
        self.c_lib.CTCAW_init_detail(progid,procspercomm, self.DEF_MAXNUMAREAS, self.DEF_MAXINTPARAMS)
        self.CTCA_get_subcomm()
        self.subrank=self.CTCA_subcomm.Get_rank()
    

    def CTCAW_pollreq(self,intparamsnum,datanum=0,datatype=0):

        start_time=MPI.Wtime()

        # prepare parameters 
        data=np.array([],dtype=np.int32)
        intparam=np.zeros(intparamsnum,dtype=np.int32)

        fromrank_c = ctypes.c_int()
        intparam_c = intparam.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
        intparamnum_c = ctypes.c_int(intparamsnum)
        
        if datanum==0 or datatype==0:

            self.c_lib.wrk_pollreq(
                ctypes.byref(fromrank_c),
                intparam_c,
                intparamnum_c,
                None,
                datanum
            )
            return fromrank_c.value,intparam
        
        elif datatype==self.DAT_INT:

            data=np.zeros(datanum,dtype=np.int32)
            datasz=datanum*np.dtype(np.int32).itemsize
            data_c = data.ctypes.data_as(ctypes.c_void_p)
            self.c_lib.wrk_pollreq(
                ctypes.byref(fromrank_c),
                intparam_c,
                intparamnum_c,
                data_c,
                datasz
            )
            
        elif datatype==self.DAT_REAL4:
            
            data=np.zeros(datanum,dtype=np.float32)
            datasz=datanum*np.dtype(np.float32).itemsize
            data_c = data.ctypes.data_as(ctypes.c_void_p)
            self.c_lib.wrk_pollreq(
                ctypes.byref(fromrank_c),
                intparam_c,
                intparamnum_c,
                data_c,
                datasz
            )
            
        elif datatype==self.DAT_REAL8:
            
            data=np.zeros(datanum,dtype=np.float64)
            datasz=datanum*np.dtype(np.float64).itemsize
            # print(f"datanum:{datanum}, datasz:{datasz}")
            data_c = data.ctypes.data_as(ctypes.c_void_p)
            self.c_lib.wrk_pollreq(
                ctypes.byref(fromrank_c),
                intparam_c,
                intparamnum_c,
                data_c,
                ctypes.c_size_t(datasz)
            )
        
        end_time=MPI.Wtime()
        time_pollreq=end_time-start_time
        
        self.WRK_pollreq_t+=time_pollreq
        self.CTCAW_pollreq_counter+=1
        # print(f"the time of poll request of python process{self.rank}: {time_pollreq:8.3e} sec")
        print(f"the time{self.CTCAW_pollreq_counter}: {time_pollreq:8.3e} sec")

        return fromrank_c.value,intparam,data
    
    def CTCAW_regarea(self,datatype=0)->int:
        areaid=ctypes.c_int(0)

        if datatype==self.AREA_INT:
            self.c_lib.CTCAW_regarea_int(ctypes.pointer(areaid))
            return areaid
        elif datatype==self.AREA_REAL4:
            self.c_lib.CTCAW_regarea_real4(ctypes.pointer(areaid))
            return areaid
        elif datatype==self.AREA_REAL8:
            self.c_lib.CTCAW_regarea_real8(ctypes.pointer(areaid))
            return areaid
        
    def CTCAW_readarea(self,areaid,reqrank,offset,size,datatype=0)->np.array:
        data=np.array([],dtype=np.int32)
        
        # start 
        start_time=MPI.Wtime()
        if datatype==self.DAT_INT:
            data=np.zeros(size,dtype=np.int32)
            self.c_lib.CTCAW_readarea_int(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))
            
        elif datatype==self.DAT_REAL4:
            data=np.zeros(size,dtype=np.float32)
            self.c_lib.CTCAW_readarea_real4(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_float)))
            
        elif datatype==self.DAT_REAL8:
            data=np.zeros(size,dtype=np.float64)
            self.c_lib.CTCAW_readarea_real8(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))
        
        # end    
        end_time=MPI.Wtime()

        self.WRK_readarea_t+=end_time-start_time

        return data
    
    def CTCAW_writearea(self,areaid,reqrank,offset,size,data,datatype):

        if not data.flags['C_CONTIGUOUS']:
            data=np.ascontiguousarray(data)
        

        if datatype==self.AREA_INT:
            data=np.zeros(size,dtype=np.int32)
            self.c_lib.CTCAW_writearea_int(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))
            
        elif datatype==self.AREA_REAL4:
            data=np.zeros(size,dtype=np.float32)
            self.c_lib.CTCAW_writearea_real4(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_float)))
            
        elif datatype==self.AREA_REAL4:
            data=np.zeros(size,dtype=np.float64)
            self.c_lib.CTCAW_writearea_real8(areaid,reqrank,\
                                            offset,size,\
                                            data.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))

    def CTCAW_isfin(self):
        return self.c_lib.CTCAW_isfin()
    
    def CTCAW_complete(self):
        return self.c_lib.CTCAW_complete()
    
    def CTCAW_finalize(self):
        return self.c_lib.CTCAW_finalize()
    
    def CTCAW_prof_start(self):
        return self.c_lib.CTCAW_prof_start()
    
    def CTCAW_prof_stop(self):
        return self.c_lib.CTCAW_prof_stop()
    
    def CTCAW_prof_start_calc(self):
        return self.c_lib.CTCAW_prof_start_calc()
    
    def CTCAW_prof_stop_calc(self):
        return self.c_lib.CTCAW_prof_stop_calc()
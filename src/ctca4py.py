import mpi4py.MPI as MPI
import numpy as np

class CTCAWorker:

    # tags of role
    ROLE_REQ = 0
    ROLE_CPL = 1
    ROLE_WRK = 2

    MAX_TRANSFER_SIZE = 1024 * 1024 * 1024
    DEF_MAXNUMAREAS = 10
    DEF_MAXINTPARAMS = 10

    # status of each process
    STAT_FIN = 0
    STAT_IDLE = 1
    STAT_RUNNING = 2

    # 
    REQSTAT_IDLE = -1

    # status of each worker subcommunicator 
    WRKSTAT_IDLE = 0
    WRKSTAT_BUSY = 1

    # BUF_AVAIL = 0
    # BUF_INUSE = 1

    # tags of sending and recieving
    TAG_REQ = 10    # request
    TAG_DAT = 20    # send data
    TAG_DATCNT = 20 # data continue
    TAG_REP = 30    # reply from requester
    TAG_FIN = 40    # finish from requester
    TAG_PROF = 50

    # 
    REQCPL_REQ_OFFSET_ENTRY = 0
    REQCPL_REQ_OFFSET_DATASIZE = 4

    # tags of area's type
    AREA_INT=0
    AREA_REAL4=1
    AREA_REAL8=2

    CPLWRK_REQ_OFFSET_FROMRANK = 0
    CPLWRK_REQ_OFFSET_PROGID = 4
    CPLWRK_REQ_OFFSET_ENTRY = 8
    CPLWRK_REQ_OFFSET_INTPARAMNUM = 12
    CPLWRK_REQ_OFFSET_DATASIZE = 16
    CPLWRK_REQ_OFFSET_DATBUFENTRY = 24
    CPLWRK_REQ_SIZE = 28

    # index for profiling
    PROF_WRK_ITEMNUM = 6
    PROF_WRK_CALC = 0 
    PROF_WRK_REGAREA=1 
    PROF_WRK_READAREA=2 
    PROF_WRK_WRITEAREA=3 
    PROF_WRK_POLLREQ=4
    PROF_WRK_COMPLETE=5 

    def __init__(self):
        # MPI.Init()
        self.CTCA_subcomm = MPI.COMM_WORLD
        self.world_myrank = MPI.COMM_WORLD.Get_rank()
        self.world_nprocs = MPI.COMM_WORLD.Get_size()

        # self.CTCA_subcomm = MPI.COMM_WORLD
        # self.world_myrank = 1
        # self.world_nprocs = 1

        self.myrole = self.ROLE_WRK
        self.mystat = None
        self.maxareas = self.DEF_MAXNUMAREAS
        self.maxintparams = self.DEF_MAXINTPARAMS
        self.numrequesters = 0
        self.areaidctr = 0
        self.rank_cpl = None

        # self.req_maxreqs = None
        # self.req_reqid_ctr = 0
        # self.req_numwrkgrps = None
        # self.req_wrkmaster_table = None
        # self.cpl_maxreqs = None
        # self.cpl_numwrkcomms = None
        # self.cpl_runrequesters = None
        # self.cpl_reqq_tail = None
        # self.cpl_datbuf_slotnum = None
        # self.cpl_datbuf_slotsz = None

        self.wrk_myworkcomm = None
        self.wrk_fromrank = None
        self.wrk_entry = None
        self.win_reqstat = None

        self.areainfo_table = None
        self.areawin_table = None
        self.subrank_table = np.zeros(self.world_nprocs, dtype=np.int32)
        self.role_table = np.zeros(self.world_nprocs, dtype=np.int32)
        self.requesterid_table = np.zeros(self.world_nprocs, dtype=np.int32)

        # self.req_reqstat_table = None
        # self.req_reqbuf = None
        # self.cpl_reqq = None
        # self.cpl_reqbuf = None
        # self.cpl_datbuf = None
        # self.cpl_datbuf_stat = None
        # self.cpl_wrkcomm_progid_table = None
        # self.cpl_wrkcomm_headrank_table = None
        # self.cpl_wrkcomm_stat_table = None

        self.wrk_reqbuf = None
        # self.cpl_reqinfo_entry_mask = (1 << 12) - 1
        self.areainfo_itemsize = None
        self.reqcpl_req_itemsize = None
        self.cplwrk_req_itemsize = None
        
        self.prof_flag = 0
        self.prof_print_flag = 0
        self.prof_total_flag = 0
        self.prof_calc_flag = 0
        self.prof_total_stime = 0
        self.prof_calc_stime = 0

        self.prof_req_times = np.zeros(6, dtype=np.float64)
        self.prof_cpl_times = np.zeros(6, dtype=np.float64)
        self.prof_wrk_times = np.zeros(6, dtype=np.float64)
        
        # self.prof_req_items = ["REQ calc", "REQ regarea", "REQ sendreq", "REQ wait", "REQ test", "REQ total"]
        # self.prof_cpl_items = ["CPL calc", "CPL regarea", "CPL readarea", "CPL writearea", "CPL pollreq", "CPL enqreq"]
        # self.prof_wrk_items = ["WRK calc", "WRK regarea", "WRK readarea", "WRK writearea", "WRK pollreq", "WRK complete"]

    def setup_common_tables(self):
        self.areawin_table = [MPI.WIN_NULL] * self.maxareas
        self.role_table = np.zeros(self.world_nprocs, dtype=np.int32)
        self.subrank_table = np.zeros(self.world_nprocs, dtype=np.int32)
        
        # Gather role of each rank to role_table
        MPI.COMM_WORLD.Allgather(np.array([self.myrole], dtype=np.int32), self.role_table)

        # Count number of requesters
        self.numrequesters = np.sum(self.role_table == self.ROLE_REQ)

        # create areainfo_table
        sizeof_size_t = np.dtype(np.uintp).itemsize
        sizeof_int = np.dtype(np.int32).itemsize
        self.areainfo_itemsize = sizeof_size_t * self.numrequesters + sizeof_int * self.numrequesters
        self.areainfo_table = np.zeros((self.maxareas, self.areainfo_itemsize), dtype=np.uint8)

        # create requesterid_table
        self.requesterid_table = np.zeros(self.numrequesters, dtype=np.int32)

    def init(self, progid, procspercomm):
        return self.init_detail(progid, procspercomm, self.DEF_MAXNUMAREAS, self.DEF_MAXINTPARAMS)

    def init_detail(self, progid, procspercomm, numareas, intparams):
        self.myrole = self.ROLE_WRK
        self.maxareas = numareas
        self.maxintparams = intparams

        # MPI.Init()
        self.world_nprocs = MPI.COMM_WORLD.Get_size()
        self.world_myrank = MPI.COMM_WORLD.Get_rank()

        self.setup_common_tables()

        self.areaidctr = 0

        # 1. Find rank of coupler
        self.rank_cpl = np.where(self.role_table == self.ROLE_CPL)[0][0]

        # 2. Attend window creation for request status
        size_byte = 0
        self.win_reqstat = MPI.Win.Create(None, MPI.INT64_T.Get_size(), MPI.INFO_NULL, MPI.COMM_WORLD)
        self.win_reqstat.Lock_all(MPI.MODE_NOCHECK)

        # 3 Attend gathering information of workers
        rank_progid_table = np.zeros(self.world_nprocs, dtype=np.int32)
        rank_procspercomm_table = np.zeros(self.world_nprocs, dtype=np.int32)
        MPI.COMM_WORLD.Allgather(np.array([progid], dtype=np.int32), rank_progid_table)
        MPI.COMM_WORLD.Allgather(np.array([procspercomm], dtype=np.int32), rank_procspercomm_table)

        # 4 Split communicator
        rank_wrkcomm_table = np.zeros(self.world_nprocs, dtype=np.int32)
        MPI.COMM_WORLD.Bcast(rank_wrkcomm_table, root=self.rank_cpl)
        self.wrk_myworkcomm = rank_wrkcomm_table[self.world_myrank]

        self.CTCA_subcomm = MPI.COMM_WORLD.Split(self.wrk_myworkcomm, 0)
        
        sub_myrank = self.CTCA_subcomm.Get_rank()
        MPI.COMM_WORLD.Allgather(np.array([sub_myrank], dtype=np.int32), self.subrank_table)

        # 5 Prepare buffer for incoming request
        self.cplwrk_req_itemsize = self.CPLWRK_REQ_SIZE + self.maxintparams * np.dtype(np.int32).itemsize
        self.wrk_reqbuf = np.zeros(self.cplwrk_req_itemsize, dtype=np.uint8)
        
        self.wrk_reqbuf[0:4] = np.full(4,-1,dtype=np.uint8,order='C')
        self.wrk_reqbuf[4:8] = np.full(4,-1,dtype=np.uint8,order='C')
        self.wrk_reqbuf[8:12] = np.full(4,-1,dtype=np.uint8,order='C')
        self.wrk_reqbuf[12:16] = np.full(4,-1,dtype=np.uint8,order='C')
        self.wrk_reqbuf[16:24] = np.full(8,-1,dtype=np.uint8,order='C')
        self.wrk_reqbuf[24:28] = np.full(4,-1,dtype=np.uint8,order='C')

        # 6 Setup requester id table
        for i in range(self.world_nprocs):
            if self.role_table[i] == self.ROLE_REQ:
                self.requesterid_table[self.subrank_table[i]] = i

        self.mystat = self.STAT_IDLE  # STAT_IDLE
        return 0
    
    def pollreq(self, intparamnum):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_pollreq() : ERROR : wrong role {self.myrole}")
        fromrank = np.zeros(1, dtype=np.int32)
        intparam = np.zeros(intparamnum, dtype=np.int32)
        self.wrk_pollreq(fromrank, intparam, intparamnum, None, 0)
        return fromrank[0], intparam

    def pollreq_withint(self, intparamnum, datanum):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_pollreq_withint() : ERROR : wrong role {self.myrole}")
        fromrank = np.zeros(1, dtype=np.int32)
        intparam = np.zeros(intparamnum, dtype=np.int32)
        data = np.zeros(datanum, dtype=np.int32)
        self.wrk_pollreq(fromrank, intparam, intparamnum, data, datanum * np.dtype(np.int32).itemsize)
        return fromrank[0], intparam, data

    def pollreq_withreal4(self, intparamnum, datanum):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_pollreq_withreal4() : ERROR : wrong role {self.myrole}")
        fromrank = np.zeros(1, dtype=np.int32)
        intparam = np.zeros(intparamnum, dtype=np.int32)
        data = np.zeros(datanum, dtype=np.float32)
        self.wrk_pollreq(fromrank, intparam, intparamnum, data, datanum * np.dtype(np.float32).itemsize)
        return fromrank[0], intparam, data

    def pollreq_withreal8(self, intparamnum, datanum):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_pollreq_withreal8() : ERROR : wrong role {self.myrole}")
        fromrank = np.zeros(1, dtype=np.int32)
        intparam = np.zeros(intparamnum, dtype=np.int32)
        data = np.zeros(datanum, dtype=np.float64)
        self.wrk_pollreq(fromrank, intparam, intparamnum, data, datanum * np.dtype(np.float64).itemsize)
        return fromrank[0], intparam, data

    def wrk_pollreq(self, fromrank, intparam, intparamnum, data, datasz):
        stat = MPI.Status()
        submyrank = self.CTCA_subcomm.Get_rank()
        subnprocs = self.CTCA_subcomm.Get_size()
        reqsize = 0
        reqintparamnum = 0
        reqdatasize = 0
        size_remain = 0
        size_tobcast = 0
        tgt_addr = None
        t0 = 0

        if self.prof_flag == 1:
            t0 = MPI.Wtime()

        if self.mystat == self.STAT_FIN:
            raise RuntimeError(f"{self.world_myrank} : wrk_pollreq() : ERROR : worker is already in FIN status")
        
        if self.mystat == self.STAT_RUNNING:
            raise RuntimeError(f"{self.world_myrank} : wrk_pollreq() : ERROR : worker is already in RUNNING status")

        if submyrank == 0:
            # print("py 1")
            MPI.COMM_WORLD.Probe(source=self.rank_cpl, tag=MPI.ANY_TAG, status=stat)
            # MPI.COMM_WORLD.Probe(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=stat)
            # print("py 2")
            # recieve request
            if stat.Get_tag() == self.TAG_REQ:
                reqsize = stat.Get_count(MPI.BYTE)
                MPI.COMM_WORLD.Recv(self.wrk_reqbuf, source=stat.Get_source(), tag=stat.Get_tag(), status=stat)
                reqintparamnum = int.from_bytes(self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_INTPARAMNUM:self.CPLWRK_REQ_OFFSET_INTPARAMNUM + 4], byteorder='little', signed=True)
                if reqintparamnum != intparamnum:
                    raise RuntimeError(f"{self.world_myrank} : wrk_pollreq() : ERROR : inconsistent number of integer parameters")
                intparam[:] = np.frombuffer(self.wrk_reqbuf[self.CPLWRK_REQ_SIZE:self.CPLWRK_REQ_SIZE + reqintparamnum * 4], dtype=np.int32)
            elif stat.Get_tag() == self.TAG_FIN:
                MPI.COMM_WORLD.Recv(np.zeros(1, dtype=np.int32), source=stat.Get_source(), tag=stat.Get_tag(), status=stat)
                # self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_PROGID:self.CPLWRK_REQ_OFFSET_PROGID + 4] = (-1).to_bytes(4, byteorder='little', signed=True)
                # self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_PROGID:self.CPLWRK_REQ_OFFSET_PROGID + 4] = -1
                self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_PROGID:self.CPLWRK_REQ_OFFSET_PROGID + 4] = np.full(4,-1,dtype=np.uint8,order='C')

            else:
                raise RuntimeError(f"{self.world_myrank} : wrk_pollreq() : ERROR : wrong tag")
        # 
        self.CTCA_subcomm.Bcast(self.wrk_reqbuf, root=0)

        if int.from_bytes(self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_PROGID:self.CPLWRK_REQ_OFFSET_PROGID + 4], byteorder='little', signed=True) != -1:
            reqdatasize = int.from_bytes(self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_DATASIZE:self.CPLWRK_REQ_OFFSET_DATASIZE + 8], byteorder='little', signed=False)
            if reqdatasize != datasz:
                raise RuntimeError(f"{self.world_myrank} : wrk_pollreq() : ERROR : data size is wrong")
            if reqdatasize > 0:
                if submyrank == 0:
                    self.recvdata(data, reqdatasize, stat.Get_source(), self.TAG_DAT, MPI.COMM_WORLD)
                size_remain = reqdatasize
                tgt_addr = data
                while size_remain > 0:
                    size_tobcast = min(size_remain, self.MAX_TRANSFER_SIZE)
                    self.CTCA_subcomm.Bcast(tgt_addr[:size_tobcast], root=0)
                    tgt_addr = tgt_addr[size_tobcast:]
                    size_remain -= size_tobcast

            intparam[:] = np.frombuffer(self.wrk_reqbuf[self.CPLWRK_REQ_SIZE:self.CPLWRK_REQ_SIZE + intparamnum * 4], dtype=np.int32)
            self.wrk_fromrank = int.from_bytes(self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_FROMRANK:self.CPLWRK_REQ_OFFSET_FROMRANK + 4], byteorder='little', signed=True)
            fromrank[0] = self.subrank_table[self.wrk_fromrank]
            self.wrk_entry = int.from_bytes(self.wrk_reqbuf[self.CPLWRK_REQ_OFFSET_ENTRY:self.CPLWRK_REQ_OFFSET_ENTRY + 4], byteorder='little', signed=True)
            self.mystat = self.STAT_RUNNING
        else:
            self.mystat = self.STAT_FIN

        if self.prof_flag == 1:
            self.prof_wrk_times[self.PROF_WRK_POLLREQ] += MPI.Wtime() - t0


    def recvdata(self, buf, size, src, tag_orig, comm):
        size_remain = size
        addr = buf
        tag = tag_orig
        while size_remain > 0:
            size_torecv = min(size_remain, self.MAX_TRANSFER_SIZE)
            comm.Recv(addr[:size_torecv], source=src, tag=tag)
            size_remain -= size_torecv
            addr = addr[size_torecv:]
            if tag == self.TAG_DAT:
                tag = self.TAG_DATCNT
        

    def regarea_int(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_regarea_int() : ERROR : wrong role {self.myrole}")
        areaid=np.zeros(1,dtype=np.int32)
        # areaid = self.areaidctr
        self.wrk_regarea(areaid)
        return areaid[0]

    def regarea_real4(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_regarea_real4() : ERROR : wrong role {self.myrole}")
        areaid=np.zeros(1,dtype=np.int32)
        # areaid = self.areaidctr
        self.wrk_regarea(areaid)
        return areaid[0]

    def regarea_real8(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_regarea_real8() : ERROR : wrong role {self.myrole}")
        areaid=np.zeros(1,dtype=np.int32)
        # areaid = self.areaidctr
        self.wrk_regarea(areaid)
        return areaid[0]
    
    def wrk_regarea(self, areaid):
        size_byte = 0
        val = 0
        # areainfo_item = self.areainfo_table[self.areaidctr]
        t0 = 0

        if self.prof_flag == 1:
            t0 = MPI.Wtime()

        # self.areawin_table[self.areaidctr] = MPI.Win.Create(val, size_byte, 4, MPI.INFO_NULL, MPI.COMM_WORLD)
        self.areawin_table[self.areaidctr] = MPI.Win.Create(None, 4, MPI.INFO_NULL, MPI.COMM_WORLD)
        self.areawin_table[self.areaidctr].Lock_all(MPI.MODE_NOCHECK)

        MPI.COMM_WORLD.Bcast([self.areainfo_table[self.areaidctr], MPI.BYTE], root=self.requesterid_table[0])

        areaid[0] = self.areaidctr
        self.areaidctr += 1

        if self.prof_flag == 1:
            self.prof_wrk_times[self.PROF_WRK_REGAREA] += MPI.Wtime() - t0

    
    
    def readarea_int(self, areaid, reqrank, offset, size):
        dest = np.zeros(size, dtype=np.int32)
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_int() : ERROR : wrong role {self.myrole}")
        if self.readarea(areaid, reqrank, offset, size, dest, self.AREA_INT) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_int() : ERROR : failed")
        return dest

    def readarea_real4(self, areaid, reqrank, offset, size):
        dest = np.zeros(size, dtype=np.float32)
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_real4() : ERROR : wrong role {self.myrole}")
        if self.readarea(areaid, reqrank, offset, size, dest, self.AREA_REAL4) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_real4() : ERROR : failed")
        return dest

    def readarea_real8(self, areaid, reqrank, offset, size):
        dest = np.zeros(size, dtype=np.float64)
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_real8() : ERROR : wrong role {self.myrole}")
        if self.readarea(areaid, reqrank, offset, size, dest, self.AREA_REAL8) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_readarea_real8() : ERROR : failed")
        return dest
    
    def readarea(self, areaid, reqrank, offset, size, dest, type):
        t0 = 0
        if self.prof_flag == 1:
            t0 = MPI.Wtime()

        areainfo_item = self.areainfo_table[areaid]

        sizeof_size_t = np.dtype(np.uintp).itemsize
        sizeof_int = np.dtype(np.int32).itemsize
        # Calculate the type and size from the areainfo_item
        area_size = int.from_bytes(areainfo_item[reqrank * sizeof_size_t : (reqrank + 1) * sizeof_size_t], byteorder='little')
        area_type = int.from_bytes(areainfo_item[self.numrequesters * sizeof_size_t + reqrank * sizeof_int : self.numrequesters * sizeof_size_t + (reqrank + 1) * sizeof_int], byteorder='little')
        
        if area_type != type:
            raise RuntimeError(f"{self.world_myrank} : readarea() : ERROR : area type is wrong")

        if area_size < size + offset:
            raise RuntimeError(f"{self.world_myrank} : readarea() : ERROR : out of range")
        
        targetrank = self.requesterid_table[reqrank]
        win = self.areawin_table[areaid]
        disp = offset
        if type == self.AREA_INT:
            mpitype = MPI.INT
            unitsize = 4
        elif type == self.AREA_REAL4:
            mpitype = MPI.FLOAT
            unitsize = 4
        elif type == self.AREA_REAL8:
            mpitype = MPI.DOUBLE
            unitsize = 8
        else:
            raise RuntimeError(f"{self.world_myrank} : readarea() : ERROR : wrong data type")

        # size_remain = size
        # addr = dest
        # while size_remain > 0:
        #     size_toget = min(size_remain * unitsize, self.MAX_TRANSFER_SIZE) // unitsize
        #     win.Get(addr, targetrank, disp, size_toget, mpitype)
        #     size_remain -= size_toget
        #     addr += size_toget * unitsize
        #     disp += size_toget
        temp1=MPI.Wtime()

        size_remain = size
        disp = offset
        while size_remain > 0:
            size_toget = min(size_remain, self.MAX_TRANSFER_SIZE // unitsize)
            win.Get([dest[disp:disp+size_toget], mpitype], targetrank, disp * unitsize)
            size_remain -= size_toget
            disp += size_toget

        win.Flush(targetrank)

        temp2=MPI.Wtime()-temp1
        print(f"the time of win.Get():{temp2:8.3e}")
        if self.prof_flag == 1:
            if self.myrole == self.ROLE_WRK:
                self.prof_wrk_times[self.PROF_WRK_READAREA] += MPI.Wtime() - t0
            # elif self.myrole == self.ROLE_CPL:
            #     self.prof_cpl_times[self.PROF_CPL_READAREA] += MPI.Wtime() - t0
            else:
                raise RuntimeError(f"{self.world_myrank} : readarea() : ERROR : wrong role {self.myrole}")

        return 0
    
    def writearea_int(self, areaid, reqrank, offset, size, src):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_int() : ERROR : wrong role {self.myrole}")
        if self.writearea(areaid, reqrank, offset, size, src, self.AREA_INT) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_int() : ERROR : failed")

    def writearea_real4(self, areaid, reqrank, offset, size, src):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_real4() : ERROR : wrong role {self.myrole}")
        if self.writearea(areaid, reqrank, offset, size, src, self.AREA_REAL4) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_real4() : ERROR : failed")

    def writearea_real8(self, areaid, reqrank, offset, size, src):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_real8() : ERROR : wrong role {self.myrole}")
        if self.writearea(areaid, reqrank, offset, size, src, self.AREA_REAL8) < 0:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_writearea_real8() : ERROR : failed")
    
    def writearea(self, areaid, reqrank, offset, size, src, type):
        t0 = 0
        if self.prof_flag == 1:
            t0 = MPI.Wtime()

        sizeof_size_t = np.dtype(np.uintp).itemsize
        sizeof_int = np.dtype(np.int32).itemsize

        areainfo_item = self.areainfo_table[areaid]

        # Calculate the type and size from the areainfo_item
        area_size = int.from_bytes(areainfo_item[reqrank * sizeof_size_t : (reqrank + 1) * sizeof_size_t], byteorder='little')
        area_type = int.from_bytes(areainfo_item[self.numrequesters * sizeof_size_t + reqrank * sizeof_int : self.numrequesters * sizeof_size_t + (reqrank + 1) * sizeof_int], byteorder='little')

        if area_type != type:
            raise RuntimeError(f"{self.world_myrank} : writearea() : ERROR : area type is wrong")

        if area_size < size + offset:
            raise RuntimeError(f"{self.world_myrank} : writearea() : ERROR : out of range")

        targetrank = self.requesterid_table[reqrank]
        win = self.areawin_table[areaid]
        disp = offset
        if type == self.AREA_INT:
            mpitype = MPI.INT
            unitsize = 4
        elif type == self.AREA_REAL4:
            mpitype = MPI.FLOAT
            unitsize = 4
        elif type == self.AREA_REAL8:
            mpitype = MPI.DOUBLE
            unitsize = 8
        else:
            raise RuntimeError(f"{self.world_myrank} : writearea() : ERROR : wrong data type")

        size_remain = size
        addr = src
        disp = offset
        while size_remain > 0:
            size_toput = min(size_remain, self.MAX_TRANSFER_SIZE // unitsize)
            win.Put([addr[disp:disp+size_toput], mpitype], targetrank, disp * unitsize)
            size_remain -= size_toput
            disp += size_toput

        win.Flush(targetrank)

        if self.prof_flag == 1:
            if self.myrole == self.ROLE_WRK:
                self.prof_wrk_times[self.PROF_WRK_WRITEAREA] += MPI.Wtime() - t0
            # elif self.myrole == self.ROLE_CPL:
            #     self.prof_cpl_times[self.PROF_CPL_WRITEAREA] += MPI.Wtime() - t0
            else:
                raise RuntimeError(f"{self.world_myrank} : writearea() : ERROR : wrong role {self.myrole}")

        return 0
    
    def isfin(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_isfin() : ERROR : wrong role {self.myrole}")
        return self.mystat == self.STAT_FIN
    
    def complete(self):
        submyrank = self.CTCA_subcomm.Get_rank()
        subnprocs = self.CTCA_subcomm.Get_size()
        t0 = 0

        if self.prof_flag == 1:
            t0 = MPI.Wtime()

        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_complete() : ERROR : wrong role {self.myrole}")

        self.CTCA_subcomm.Barrier()

        if self.mystat == self.STAT_FIN:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_complete() : ERROR : worker is already in FIN status")

        if self.mystat == self.STAT_IDLE:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_complete() : ERROR : worker is in IDLE status")

        if submyrank == 0:
            val = self.WRKSTAT_IDLE
            MPI.COMM_WORLD.Send(np.array([val], dtype=np.int32), dest=self.rank_cpl, tag=self.TAG_REP)
            if self.wrk_entry >= 0:
                val64 = self.REQSTAT_IDLE
                disp = self.wrk_entry
                self.win_reqstat.Put(np.array([val64], dtype=np.int64).tobytes(), target=self.wrk_fromrank, target_disp=disp)
                self.win_reqstat.Flush(self.wrk_fromrank)

        self.mystat = self.STAT_IDLE

        if self.prof_flag == 1:
            self.prof_wrk_times[self.PROF_WRK_COMPLETE] += MPI.Wtime() - t0

        return 0


    def finalize(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_finalize() : ERROR : wrong role {self.myrole}")

        MPI.COMM_WORLD.Barrier()

        if self.prof_print_flag == 1:
            submyrank = self.CTCA_subcomm.Get_rank()
            subnprocs = self.CTCA_subcomm.Get_size()
            if submyrank == 0:
                MPI.COMM_WORLD.Send(self.prof_wrk_times, dest=self.requesterid_table[0], tag=self.TAG_PROF)

        self.win_reqstat.Unlock_all()
        self.win_reqstat.Free()

        for i in range(self.areaidctr):
            self.areawin_table[i].Unlock_all()
            self.areawin_table[i].Free()

        self.free_common_tables()
        self.wrk_reqbuf = None

        MPI.Finalize()

        return 0
    
    def free_common_tables(self):
        self.areainfo_table = None
        self.areawin_table = None
        self.role_table = None
        self.subrank_table = None
        self.requesterid_table = None
    
    def prof_start(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_prof_start() : ERROR : wrong role {self.myrole}")
        if self.startprof() < 0:
            print(f"{self.world_myrank} : CTCAW_prof_start() : WARNING : prof_flag was not 0")

    def prof_stop(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_prof_stop() : ERROR : wrong role {self.myrole}")
        if self.stopprof() < 0:
            print(f"{self.world_myrank} : CTCAW_prof_stop() : WARNING : prof_flag was not 1")

    def startprof(self):
        if self.prof_flag != 0:
            return -1
        self.prof_flag = 1
        self.prof_print_flag = 1
        return 0

    def stopprof(self):
        if self.prof_flag != 1:
            return -1
        self.prof_flag = 0
        return 0
    
    def prof_start_calc(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_prof_start_calc() : ERROR : wrong role {self.myrole}")
        if self.prof_calc_flag != 0:
            print(f"{self.world_myrank} : CTCAW_prof_start_calc() : WARNING : prof_calc_flag was not 0")
        self.prof_calc_flag = 1
        self.prof_calc_stime = MPI.Wtime()

    def prof_stop_calc(self):
        if self.myrole != self.ROLE_WRK:
            raise RuntimeError(f"{self.world_myrank} : CTCAW_prof_stop_calc() : ERROR : wrong role {self.myrole}")
        if self.prof_calc_flag != 1:
            print(f"{self.world_myrank} : CTCAW_prof_stop_calc() : WARNING : prof_calc_flag was not 1")
        self.prof_calc_flag = 0
        self.prof_wrk_times[self.PROF_WRK_CALC] += MPI.Wtime() - self.prof_calc_stime

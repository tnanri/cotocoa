// CoToCoA (Code To Code Adapter)
//  A framework to connect a requester program to multiple worker programs via a coupler program

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

MPI_Comm CTCA_subcomm;

#define MAX_TRANSFER_SIZE (1024*1024*1024)
#define DEF_MAXNUMAREAS 10
#define DEF_REQ_NUMREQS 10
#define DEF_MAXINTPARAMS 10
#define DEF_CPL_NUMREQS 10
#define DEF_CPL_DATBUF_SLOTSZ 80000
#define DEF_CPL_DATBUF_SLOTNUM 10
#define ROLE_REQ 0
#define ROLE_CPL 1
#define ROLE_WRK 2
#define STAT_FIN 0
#define STAT_IDLE 1
#define STAT_RUNNING 2
#define REQSTAT_IDLE -1LL
#define WRKSTAT_IDLE 0
#define WRKSTAT_BUSY 1
#define BUF_AVAIL 0
#define BUF_INUSE 1
#define TAG_REQ 10
#define TAG_DAT 20
#define TAG_DATCNT 20
#define TAG_REP 30
#define TAG_FIN 40
#define TAG_PROF 50
#define AREA_INT 0
#define AREA_REAL4 1
#define AREA_REAL8 2
#define DAT_INT 0
#define DAT_REAL4 1
#define DAT_REAL8 2
#define REQCPL_REQ_OFFSET_ENTRY 0
#define REQCPL_REQ_OFFSET_DATASIZE 4
#define REQCPL_REQ_SIZE 12
#define CTCAC_REQINFOITEMS 4
#define CPL_REQINFO_ENTRY_BITS 12
#define CPL_REQINFO_OFFSET_INTPARAMNUM 4
#define CPL_REQINFO_OFFSET_DATASIZE 8
#define CPL_REQINFO_SIZE 16
#define CPLWRK_REQ_OFFSET_FROMRANK 0
#define CPLWRK_REQ_OFFSET_PROGID 4
#define CPLWRK_REQ_OFFSET_ENTRY 8
#define CPLWRK_REQ_OFFSET_INTPARAMNUM 12
#define CPLWRK_REQ_OFFSET_DATASIZE 16
#define CPLWRK_REQ_OFFSET_DATBUFENTRY 24
#define CPLWRK_REQ_SIZE 28

#define AREAINFO_SIZE(addr,reqrank)  (*((size_t *)((char *)(addr) + reqrank * sizeof(size_t))))
#define AREAINFO_TYPE(addr,reqrank)  (*((int *)((char *)(addr) + numrequesters * sizeof(size_t) + reqrank * sizeof(int))))
#define REQCPL_REQ_ENTRY(addr)  (*((int *)((char *)(addr) + REQCPL_REQ_OFFSET_ENTRY)))
#define REQCPL_REQ_DATASIZE(addr)  (*((size_t *)((char *)(addr) + REQCPL_REQ_OFFSET_DATASIZE)))
#define CPLWRK_REQ_FROMRANK(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_FROMRANK)))
#define CPLWRK_REQ_PROGID(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_PROGID)))
#define CPLWRK_REQ_ENTRY(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_ENTRY)))
#define CPLWRK_REQ_INTPARAMNUM(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_INTPARAMNUM)))
#define CPLWRK_REQ_DATASIZE(addr)  (*((size_t *)((char *)(addr) + CPLWRK_REQ_OFFSET_DATASIZE)))
#define CPLWRK_REQ_DATBUFENTRY(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_DATBUFENTRY)))

static int world_myrank, world_nprocs;
static int myrole, mystat, maxareas, rank_cpl, maxintparams;
static int numrequesters, areaidctr;
static int req_maxreqs;
static int64_t req_reqid_ctr;
static int req_numwrkgrps;
static int *req_wrkmaster_table;
static int cpl_maxreqs, cpl_numwrkcomms, cpl_runrequesters, cpl_reqq_tail;
static int cpl_datbuf_slotnum;
static size_t cpl_datbuf_slotsz;
static int wrk_myworkcomm, wrk_fromrank, wrk_entry;
static MPI_Win win_reqstat;
static size_t *areainfo_table;
static MPI_Win *areawin_table;
static int *subrank_table;
static int *role_table;
static int *requesterid_table;
static volatile int64_t *req_reqstat_table;
static size_t *req_reqbuf;
static size_t *cpl_reqq;
static size_t *cpl_reqbuf;
static double *cpl_datbuf;
static int *cpl_datbuf_stat;
static int *cpl_wrkcomm_progid_table;
static int *cpl_wrkcomm_headrank_table;
static volatile int *cpl_wrkcomm_stat_table;
static size_t *wrk_reqbuf;
static int cpl_reqinfo_entry_mask = (1<<CPL_REQINFO_ENTRY_BITS)-1;
static int areainfo_itemsize;
static int reqcpl_req_itemsize;
static int cplwrk_req_itemsize;

static int prof_flag = 0;
static int prof_print_flag = 0;
static int prof_total_flag = 0;
static int prof_calc_flag = 0;
static double prof_total_stime;
static double prof_calc_stime;

#define PROF_REQ_ITEMNUM 6
#define PROF_REQ_CALC    0
#define PROF_REQ_REGAREA 1 
#define PROF_REQ_SENDREQ 2 
#define PROF_REQ_WAIT    3 
#define PROF_REQ_TEST    4 
#define PROF_REQ_TOTAL   5

#define PROF_CPL_ITEMNUM   6
#define PROF_CPL_CALC      0 
#define PROF_CPL_REGAREA   1
#define PROF_CPL_READAREA  2
#define PROF_CPL_WRITEAREA 3 
#define PROF_CPL_POLLREQ   4
#define PROF_CPL_ENQREQ    5 

#define PROF_WRK_ITEMNUM   6
#define PROF_WRK_CALC      0 
#define PROF_WRK_REGAREA   1 
#define PROF_WRK_READAREA  2 
#define PROF_WRK_WRITEAREA 3 
#define PROF_WRK_POLLREQ   4
#define PROF_WRK_COMPLETE  5 

#define PROF_TITLE_LEN 16
double prof_req_times[PROF_REQ_ITEMNUM];
double prof_cpl_times[PROF_CPL_ITEMNUM];
double prof_wrk_times[PROF_WRK_ITEMNUM];
char prof_req_items[PROF_REQ_ITEMNUM][PROF_TITLE_LEN] = {
    "REQ calc",
    "REQ regarea",
    "REQ sendreq",
    "REQ wait",
    "REQ test",
    "REQ total"
};
char prof_cpl_items[PROF_CPL_ITEMNUM][PROF_TITLE_LEN] = {
    "CPL calc",
    "CPL regarea",
    "CPL readarea",
    "CPL writearea",
    "CPL pollreq",
    "CPL enqreq"
};
char prof_wrk_items[PROF_WRK_ITEMNUM][PROF_TITLE_LEN] = {
    "WRK calc",
    "WRK regarea",
    "WRK readarea",
    "WRK writearea",
    "WRK pollreq",
    "WRK complete"
};

static int startprof()
{
    int ret;

    ret = 0;
    if (prof_flag != 0) ret = -1;
    prof_flag = 1;
    prof_print_flag = 1;

    return ret;
}
    
static int stopprof()
{
    int ret;

    ret = 0;
    if (prof_flag != 1) ret = -1;
    prof_flag = 0;

    return ret;
}
    
static int setup_common_tables(int myrole)
{
    int i;

    areawin_table = (MPI_Win *)malloc(maxareas * sizeof(MPI_Win));
    role_table = (int *)malloc(world_nprocs * sizeof(int));
    subrank_table = (int *)malloc(world_nprocs * sizeof(int));

    //  gather role of each rank
    MPI_Allgather(&myrole, 1, MPI_INT, role_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  count number of requesters
    numrequesters = 0;
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            numrequesters++;
    areainfo_itemsize = sizeof(size_t) * numrequesters + sizeof(int) * numrequesters;
    areainfo_table = (size_t *)malloc(areainfo_itemsize * maxareas);
    requesterid_table = (int *)malloc(numrequesters * sizeof(int));

    return 0;
}

static int free_common_tables()
{
    free(areainfo_table);
    free(areawin_table);
    free(role_table);
    free(subrank_table);
    free(requesterid_table);

    return 0;
}

int senddata(void *buf, size_t size, int dest, int tag_orig, MPI_Comm comm)
{
    size_t size_remain;
    int size_tosend;
    void *addr;
    int tag;

    size_remain = size;
    addr = buf;
    tag = tag_orig;
    while (size_remain > 0) {
        size_tosend = (size_remain > MAX_TRANSFER_SIZE) ? MAX_TRANSFER_SIZE : size_remain;
        MPI_Send(addr, size_tosend, MPI_BYTE, dest, tag, comm);
        size_remain -= size_tosend;
        addr += size_tosend;
        if (tag == TAG_DAT)
            tag = TAG_DATCNT;
    }

    return 0;
}

int recvdata(void *buf, size_t size, int src, int tag_orig, MPI_Comm comm)
{
    size_t size_remain, size_torecv;
    void *addr;
    int tag;

    size_remain = size;
    addr = buf;
    tag = tag_orig;
    while (size_remain > 0) {
        size_torecv = (size_remain > MAX_TRANSFER_SIZE) ? MAX_TRANSFER_SIZE : size_remain;
        MPI_Recv(addr, size_torecv, MPI_BYTE, src, tag, comm, MPI_STATUS_IGNORE);
        size_remain -= size_torecv;
        addr += size_torecv;
        if (tag == TAG_DAT)
            tag = TAG_DATCNT;
    }

    return 0;
}

int CTCAR_init_detail(int numareas, int numreqs, int intparams)
{
    int i, sub_myrank, val, ctr;
    int *rank_progid_table;
    int *rank_procspercomm_table;
    int *rank_wrkcomm_table;
    MPI_Aint size_byte;

    myrole = ROLE_REQ;
    maxareas = numareas;

    if (numreqs >= cpl_reqinfo_entry_mask) {
        fprintf(stderr, "CTCAR_init_detail : ERROR : numreqs must be less than %d\n", cpl_reqinfo_entry_mask);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    req_maxreqs = numreqs;
    maxintparams = intparams;
    mystat = STAT_RUNNING;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_myrank);

    setup_common_tables(myrole);

    areaidctr = 0;

    //  setup request status table
    size_byte = sizeof(int64_t) * req_maxreqs;
    MPI_Alloc_mem(size_byte, MPI_INFO_NULL, &req_reqstat_table);
    for (i = 0; i < req_maxreqs; i++)
        req_reqstat_table[i] = REQSTAT_IDLE;

    MPI_Win_create((void *)req_reqstat_table, size_byte, sizeof(int64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(MPI_MODE_NOCHECK, win_reqstat);

    //  setup a buffer for outgoing request
    reqcpl_req_itemsize = REQCPL_REQ_SIZE + maxintparams * sizeof(int);
    req_reqbuf = (size_t *)malloc(reqcpl_req_itemsize);

    //  find rank of coupler
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_CPL) {
            rank_cpl = i;
            break;
        }

    //  attend gathering information of workers
    rank_progid_table = (int *)malloc(world_nprocs * sizeof(int));
    rank_procspercomm_table = (int *)malloc(world_nprocs * sizeof(int));
    val = -1;
    MPI_Allgather(&val, 1, MPI_INT, rank_progid_table, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&val, 1, MPI_INT, rank_procspercomm_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  initialize reqids
    req_reqid_ctr = 1;

    //  split communicator
    rank_wrkcomm_table = (int *)malloc(world_nprocs * sizeof(int));
    MPI_Bcast(rank_wrkcomm_table, world_nprocs, MPI_INT, rank_cpl, MPI_COMM_WORLD);
    MPI_Comm_split(MPI_COMM_WORLD, rank_wrkcomm_table[world_myrank], 0, &CTCA_subcomm);
    MPI_Comm_rank(CTCA_subcomm, &sub_myrank);
    MPI_Allgather(&sub_myrank, 1, MPI_INT, subrank_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  setup requester id table (used for converting subrank of the requester to world rank)
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            requesterid_table[subrank_table[i]] = i;

    // setup worker masters table (used for gathering profiling data)
    req_numwrkgrps = 0;
    for (i = 0; i < world_nprocs; i++)
        if ((role_table[i] == ROLE_WRK) && (subrank_table[i] == 0))
            req_numwrkgrps++;

    req_wrkmaster_table = (int *)malloc(req_numwrkgrps * sizeof(int));
    ctr = 0;
    for (i = 0; i < world_nprocs; i++)
        if ((role_table[i] == ROLE_WRK) && (subrank_table[i] == 0)) {
            req_wrkmaster_table[ctr] = i;
            ctr++;
        }

    free(rank_progid_table);
    free(rank_procspercomm_table);
    free(rank_wrkcomm_table);

    return 0;
}

int CTCAR_init()
{
    return CTCAR_init_detail(DEF_MAXNUMAREAS, DEF_REQ_NUMREQS, DEF_MAXINTPARAMS);
}

int req_regarea(void *base, size_t size, MPI_Aint size_byte, int unit, int type, int *areaid)
{
    char *areainfo_item;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    areainfo_item = (char *)areainfo_table + areaidctr * areainfo_itemsize;

    //  Create a window for this area
    MPI_Win_create(base, size_byte, unit, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(MPI_MODE_NOCHECK, areawin_table[areaidctr]);

    //  Set and broadcast areainfo_table
    //  Gather sizes and types to requester 0
    MPI_Gather(&size, 8, MPI_BYTE, areainfo_item, 8, MPI_BYTE, 0, CTCA_subcomm);
    MPI_Gather(&type, 1, MPI_INT, areainfo_item + numrequesters * sizeof(size_t), 1, MPI_INT, 0, CTCA_subcomm);

    //  Broadcast areainfo
    MPI_Bcast(areainfo_item, areainfo_itemsize, MPI_BYTE, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_REGAREA] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_regarea_int(int *base, size_t size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    size_byte = size * sizeof(int);
    return req_regarea((void *)base, size, size_byte, sizeof(int), AREA_INT, areaid);
}

int CTCAR_regarea_real4(float *base, size_t size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_real4() : ERROR : wrong role\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    size_byte = size * sizeof(float);
    return req_regarea((void *)base, size, size_byte, sizeof(float), AREA_REAL4, areaid);
}

int CTCAR_regarea_real8(double *base, size_t size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    size_byte = size * sizeof(double);
    return req_regarea((void *)base, size, size_byte, sizeof(double), AREA_REAL8, areaid);
}

int req_sendreq(int *intparams, int numintparams, void *data, size_t datasize, int datatype, int reqentry)
{
    int reply;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : req_sendreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (numintparams > maxintparams) {
        fprintf(stderr, "%d : req_sendreq() : ERROR : numintparams is too large\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  setup a request message
    REQCPL_REQ_ENTRY(req_reqbuf) = reqentry;
    REQCPL_REQ_DATASIZE(req_reqbuf) = datasize;

    memcpy((char *)req_reqbuf + REQCPL_REQ_SIZE, intparams, numintparams * sizeof(int));

    //  send a request (with integer data) to the coupler
    MPI_Send(req_reqbuf, REQCPL_REQ_SIZE + numintparams * sizeof(int), MPI_BYTE, rank_cpl, TAG_REQ, MPI_COMM_WORLD);
    if (datasize > 0) 
        senddata(data, datasize, rank_cpl, TAG_DAT, MPI_COMM_WORLD);

    //  wait for a reply from the coupler
    MPI_Recv(&reply, 1, MPI_INT, rank_cpl, TAG_REP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return 0;
}

int CTCAR_sendreq(int *intparams, int numintparams)
{
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    req_sendreq(intparams, numintparams, NULL, 0, 0, -1);

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int find_reqentry()
{
    int i, ret;

    ret = -1;
    for (i = 0; i < req_maxreqs; i++)
        if (req_reqstat_table[i] == REQSTAT_IDLE) {
            ret = i;
            break;
        }

    return ret;
}

int CTCAR_sendreq_hdl(int *intparams, int numintparams, int64_t *hdl)
{
    int reqentry;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_hdl() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
        //  couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_hdl() : ERROR : req_reqstat_table is full\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    } else {
        //  set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
        //  also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
        //  increment request id
        req_reqid_ctr++;

        //  send a request with this entry
        req_sendreq(intparams, numintparams, NULL, 0, 0, reqentry);
    }

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withint(int *intparams, int numintparams, int *data, size_t datanum)
{
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withint() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(int), DAT_INT, -1);

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withreal4(int *intparams, int numintparams, float *data, size_t datanum)
{
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(float), DAT_REAL4, -1);

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withreal8(int *intparams, int numintparams, double *data, size_t datanum)
{
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(double), DAT_REAL8, -1);

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withint_hdl(int *intparams, int numintparams, int *data, size_t datanum, int64_t *hdl)
{
    int reqentry;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withint_hdl() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
        //  couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withint_hdl() : ERROR : req_reqstat_table is full\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    } else {
        //  set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
        //  also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
        //  increment request id
        req_reqid_ctr++;
        //  send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(int), DAT_INT, reqentry);
    }

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withreal4_hdl(int *intparams, int numintparams, float *data, size_t datanum, int64_t *hdl)
{
    int reqentry;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4_hdl() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
        //  couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4_hdl() : ERROR : req_reqstat_table is full\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    } else {
        //  set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
        //  also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
        //  increment request id
        req_reqid_ctr++;
        //  send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(float), DAT_REAL4, reqentry);
    }

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_sendreq_withreal8_hdl(int *intparams, int numintparams, double *data, size_t datanum, int64_t *hdl)
{
    int reqentry;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8_hdl() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
        //  couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8_hdl() : ERROR : req_reqstat_table is full\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    } else {
        //  set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
        //  also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
        //  increment request id
        req_reqid_ctr++;
        //  send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(double), DAT_REAL8, reqentry);
    }

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_SENDREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_wait(int64_t hdl)
{
    int i, flag, f;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_wait() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    do {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &f, MPI_STATUS_IGNORE);
        flag = 1;
        for (i = 0; i < req_maxreqs; i++)
            if (req_reqstat_table[i] == hdl) {
                flag = 0;
                break;
            }
    } while (flag == 0);

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_WAIT] += MPI_Wtime() - t0;

    return 0;
}

int CTCAR_test(int64_t hdl)
{
    int i, flag;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_test() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    flag = 1;
    for (i = 0; i < req_maxreqs; i++)
        if (req_reqstat_table[i] == hdl) {
            flag = 0;
            break;
        }

    if (prof_flag == 1) 
        prof_req_times[PROF_REQ_TEST] += MPI_Wtime() - t0;

    return flag;
}

int CTCAR_finalize()
{
    int val, i, j, submyrank, subnprocs;
    double times_req[PROF_REQ_ITEMNUM];
    double times_cpl[PROF_CPL_ITEMNUM];
    double times_wrk[PROF_WRK_ITEMNUM];

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_finalize() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    mystat = STAT_FIN;
    MPI_Send(&val, 1, MPI_INT, rank_cpl, TAG_FIN, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm_rank(CTCA_subcomm, &submyrank);
    MPI_Comm_size(CTCA_subcomm, &subnprocs);

    if (prof_print_flag == 1) {
        if (submyrank == 0) {
            printf("Profiling results, Requester  \n"); 
            printf("ID "); 
            for (i = 0; i < PROF_REQ_ITEMNUM; i++) 
                printf(", %s ", prof_req_items[i]);
            printf("\n");
            
            printf("   0  "); 
            for (i = 0; i < PROF_REQ_ITEMNUM; i++) 
                printf(", %8.3e ", prof_req_times[i]);
            printf("\n");
            
            for (i = 1; i < subnprocs; i++) {
                MPI_Recv(times_req, PROF_REQ_ITEMNUM, MPI_DOUBLE, i, TAG_PROF, CTCA_subcomm, MPI_STATUS_IGNORE);
//                printf("%4d , ", i); 
//                for (j = 0; j < PROF_REQ_ITEMNUM; j++) 
//                    printf(", %8.3e ", times_req[j]);
//                printf("\n");
            }
            
            printf("Profiling results, Coupler  \n"); 
            MPI_Recv(times_cpl, PROF_CPL_ITEMNUM, MPI_DOUBLE, rank_cpl, TAG_PROF, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("ID "); 
            for (i = 0; i < PROF_CPL_ITEMNUM; i++) 
                printf(", %s ", prof_cpl_items[i]);
            printf("\n");
            
            printf("   0 "); 
            for (i = 0; i < PROF_CPL_ITEMNUM; i++) 
                printf(", %8.3e ", times_cpl[i]);
            printf("\n");
            
            printf("Profiling results, Worker  \n"); 
            printf("GRP ID "); 
            for (i = 0; i < PROF_WRK_ITEMNUM; i++) 
                printf(", %s ", prof_wrk_items[i]);
            printf("\n");
            
            for (i = 0; i < req_numwrkgrps; i++) {
                MPI_Recv(times_wrk, PROF_WRK_ITEMNUM, MPI_DOUBLE, req_wrkmaster_table[i], TAG_PROF, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("%4d ", i); 
                for (j = 0; j < PROF_WRK_ITEMNUM; j++) 
                    printf(", %8.3e ", times_wrk[j]);
                printf("\n");
            }
        } else {
            MPI_Send(prof_req_times, PROF_REQ_ITEMNUM, MPI_DOUBLE, 0, TAG_PROF, CTCA_subcomm);
        }
    }
    
    MPI_Win_unlock_all(win_reqstat);
    MPI_Win_free(&win_reqstat);
    for (i = 0; i < areaidctr; i++) {
        MPI_Win_unlock_all(areawin_table[i]);
        MPI_Win_free(&(areawin_table[i]));
    }

    free_common_tables();
    MPI_Free_mem((void *)req_reqstat_table);
    free(req_reqbuf);
    free(req_wrkmaster_table);

    MPI_Finalize();

    return 0;
}

int CTCAR_prof_start()
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_prof_start() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (startprof() < 0) 
        fprintf(stderr, "%d : CTCAR_prof_start() : WARNING : prof_flag was not 0\n", world_myrank);
}

int CTCAR_prof_stop()
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_prof_stop() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (stopprof() < 0) 
        fprintf(stderr, "%d : CTCAR_prof_stop() : WARNING : prof_flag was not 1\n", world_myrank);
}

int CTCAR_prof_start_total()
{
    if (myrole != ROLE_REQ) 
        fprintf(stderr, "%d : CTCAR_prof_start_total() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_total_flag != 0) 
        fprintf(stderr, "%d : CTCAR_prof_start_total() : WARNING : prof_total_flag was not 0\n", world_myrank);

    prof_total_flag = 1;

    prof_total_stime = MPI_Wtime();
}

int CTCAR_prof_stop_total()
{
    if (myrole != ROLE_REQ) 
        fprintf(stderr, "%d : CTCAR_prof_stop_total() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_total_flag != 1) 
        fprintf(stderr, "%d : CTCAR_prof_stop_total() : WARNING : prof_total_flag was not 1\n", world_myrank);

    prof_total_flag = 0;

    prof_req_times[PROF_REQ_TOTAL] += MPI_Wtime() - prof_total_stime;
}

int CTCAR_prof_start_calc()
{
    if (myrole != ROLE_REQ) 
        fprintf(stderr, "%d : CTCAR_prof_start_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 0) 
        fprintf(stderr, "%d : CTCAR_prof_start_calc() : WARNING : prof_total_flag was not 0\n", world_myrank);

    prof_calc_flag = 1;

    prof_calc_stime = MPI_Wtime();
}

int CTCAR_prof_stop_calc()
{
    if (myrole != ROLE_REQ) 
        fprintf(stderr, "%d : CTCAR_prof_stop_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 1) 
        fprintf(stderr, "%d : CTCAR_prof_stop_total() : WARNING : prof_total_flag was not 1\n", world_myrank);

    prof_calc_flag = 0;

    prof_req_times[PROF_REQ_CALC] += MPI_Wtime() - prof_calc_stime;
}

int insert_progid(int *table, int tablesize, int id, int numentries)
{
    int i, j, ret;

    ret = numentries;

    if (id >= 0) {
        if (numentries == 0) {
            table[0] = id;
            ret = numentries + 1;
        } else {
            for (i = 0; i < numentries; i++) {
                if (id == table[i]) 
                    break;
                if (id > table[i]) {
                    if (numentries + 1 > tablesize) {
                        fprintf(stderr, "%d : insert_progid() : progid table is full\n", world_myrank);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                    } 
                    for (j = numentries; j >= i+1; j--)
                        table[j] = table[j-1];
                    table[i] = id;
                    ret = numentries+1;
                    break;
                }
            }
        }
    }
    return ret;
}

int find_progid(int *table, int tablesize, int id)
{
    int low, high, mid, ret;

    low = 0;
    high = tablesize - 1;
    ret = -1;

    while (low <= high) {
        mid = (high + low) / 2;
        if (table[mid] == id) {
            ret = mid;
            break;
        } else {
            if (table[mid] > id)
                low = mid + 1;
            else
                high = mid - 1;
        }
    } 

    return ret;
}

int CTCAC_reqinfo_get_fromrank(int *reqinfo)
{
    return (*reqinfo >> CPL_REQINFO_ENTRY_BITS);
}

void CTCAC_reqinfo_set_fromrank(int *reqinfo, int rank)
{
    *reqinfo = (*reqinfo & cpl_reqinfo_entry_mask) | (rank << CPL_REQINFO_ENTRY_BITS);
}

int CTCAC_reqinfo_get_entry(int *reqinfo)
{
    int ret = *reqinfo & cpl_reqinfo_entry_mask;
    if (ret == cpl_reqinfo_entry_mask)
        ret = -1;
    return ret;
}

void CTCAC_reqinfo_set_entry(int *reqinfo, int entry)
{
    *reqinfo = (*reqinfo & ~cpl_reqinfo_entry_mask) | (entry & cpl_reqinfo_entry_mask);
}

int CTCAC_reqinfo_get_intparamnum(int *reqinfo)
{
    return (*((int *)((char *)reqinfo + CPL_REQINFO_OFFSET_INTPARAMNUM)));
}

void CTCAC_reqinfo_set_intparamnum(int *reqinfo, int intparamnum)
{
    *((int *)((char *)reqinfo + CPL_REQINFO_OFFSET_INTPARAMNUM)) = intparamnum;
}

size_t CTCAC_reqinfo_get_datasize(int *reqinfo)
{
    return (*((size_t *)((char *)reqinfo + CPL_REQINFO_OFFSET_DATASIZE)));
}

void CTCAC_reqinfo_set_datasize(int *reqinfo, size_t datasize)
{
    *((size_t *)((char *)reqinfo + CPL_REQINFO_OFFSET_DATASIZE)) = datasize;
}

int CTCAC_init_detail(int numareas, int numreqs, int intparams, size_t bufslotsz, int bufslotnum)
{
    int i, j, p, n, cpl_numprogids, sub_myrank, val;
    int *rank_progid_table, *rank_procspercomm_table, *uniq_progid_table;
    int *progid_table, *progid_numcomms_table, *progid_wrkcommctr_table;
    int * progid_numprocs_table, *progid_procspercomm_table, *rank_wrkcomm_table;
    int *progid_procctr_table;
    MPI_Aint size_byte;

    myrole = ROLE_CPL;
    maxareas = numareas;
    cpl_maxreqs = numreqs;
    maxintparams = intparams;
    cpl_datbuf_slotsz = bufslotsz;
    cpl_datbuf_slotnum = bufslotnum;
    mystat = STAT_RUNNING;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_myrank);

    setup_common_tables(myrole);

    areaidctr = 0;

    rank_cpl = world_myrank;

    cpl_runrequesters = numrequesters;

    //  Attend win_create for request status table on requesters
    size_byte = 0;
    MPI_Win_create(0, size_byte, sizeof(int64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(MPI_MODE_NOCHECK, win_reqstat);

    //  setup a buffer for incomming requests
    reqcpl_req_itemsize = REQCPL_REQ_SIZE + maxintparams * sizeof(int);
    cpl_reqbuf = (size_t *)malloc(reqcpl_req_itemsize);

    //  setup a queue for outgoing requests
    cplwrk_req_itemsize = CPLWRK_REQ_SIZE + maxintparams * sizeof(int);
    cpl_reqq = (size_t *)malloc(cplwrk_req_itemsize * (cpl_maxreqs + 1));
    for (i = 0; i < cpl_maxreqs + 1; i++) {
        CPLWRK_REQ_FROMRANK((char *)cpl_reqq + i * cplwrk_req_itemsize) = -1;
        CPLWRK_REQ_PROGID((char *)cpl_reqq + i * cplwrk_req_itemsize) = -1;
        CPLWRK_REQ_ENTRY((char *)cpl_reqq + i * cplwrk_req_itemsize) = -1;
        CPLWRK_REQ_INTPARAMNUM((char *)cpl_reqq + i * cplwrk_req_itemsize) = -1;
        CPLWRK_REQ_DATASIZE((char *)cpl_reqq + i * cplwrk_req_itemsize) = 0;
        CPLWRK_REQ_DATBUFENTRY((char *)cpl_reqq + i * cplwrk_req_itemsize) = -1;
    }
    cpl_reqq_tail = 0;

    //  setup a buffer and a status table for storing real8 data for outgoing requests
    cpl_datbuf = (double *)malloc(cpl_datbuf_slotsz * cpl_datbuf_slotnum);
    cpl_datbuf_stat = (int *)malloc(cpl_datbuf_slotnum * sizeof(int));
    for (i = 0; i < cpl_datbuf_slotnum; i++)
        cpl_datbuf_stat[i] = BUF_AVAIL;

    //  gather information of workers
    //     
    //    rank_progid_table(conn_world_nprocs)
    //      table to gather prog id of each rank
    //       
    //    rank_procspercomm_table(conn_world_nprocs)
    //      table to gather num procs per communicator of each rank
    rank_progid_table = (int *)malloc(world_nprocs * sizeof(int));
    rank_procspercomm_table = (int *)malloc(world_nprocs * sizeof(int));
    val = -1;
    MPI_Allgather(&val, 1, MPI_INT, rank_progid_table, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&val, 1, MPI_INT, rank_procspercomm_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  Setup information of worker programs
    //
    //  - pickup unique prog ids
    //    uniq_progid_table(conn_world_nprocs):
    //      temporal table to store unique prog ids in rank_progid_table
    //  
    //    cpl_numprogids:
    //      number of unique prog ids
    //  
    //    progid_table(cpl_numprogids):
    //      temporal table to store unique prog ids in rank_progid_table

    uniq_progid_table = (int *)malloc(world_nprocs * sizeof(int));
    for (i = 0; i < world_nprocs; i++)
        uniq_progid_table[i] = -1;

    cpl_numprogids = 0;
    for (i = 0; i < world_nprocs; i++){
        p = rank_progid_table[i];
        if (p >= 0) 
            cpl_numprogids = insert_progid(uniq_progid_table, world_nprocs, p, cpl_numprogids);
    }

    progid_table = (int *)malloc(cpl_numprogids * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++)
        progid_table[i] = uniq_progid_table[i];
    free(uniq_progid_table); //

    //  - calc num procs per commuincator of each prog id:
    //    progid_numprocs_table(cpl_numprogids):
    //      number of processes of each prog id
    //    progid_procspercomm_table(cpl_numprogids): 
    //      number of processes per communicator of each progid
    progid_numprocs_table = (int *)malloc(cpl_numprogids * sizeof(int));
    progid_procspercomm_table = (int *)malloc(cpl_numprogids * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++) {
        progid_numprocs_table[i] = 0;
        progid_procspercomm_table[i] = -1;
    }

    for (i = 0; i < world_nprocs; i++) {
        p = rank_progid_table[i];
        if (p >= 0) {
            j = find_progid(progid_table, cpl_numprogids, p);
            n = rank_procspercomm_table[i];
            progid_numprocs_table[j]++;
            if (progid_procspercomm_table[j] == -1)
                progid_procspercomm_table[j] = n;
            else{
                if (progid_procspercomm_table[j] != n) {
                    fprintf(stderr, "%d : CTCAC_init_detail() : ERROR : inconsistent procspercomm of rank\n", world_myrank);
                    MPI_Abort(MPI_COMM_WORLD, 0);
                }
            }
        }
    }

    //  - calc number of communicators of each prog id
    //    progid_numcomms_table(cpl_numprogids): 
    //        number of communicators of each progid
    progid_numcomms_table = (int *)malloc(cpl_numprogids * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++)
        if (progid_numprocs_table[i] % progid_procspercomm_table[i] != 0) {
            fprintf(stderr, "%d : CTCAC_init_detail() : ERROR : num procs per prog id is not dividable by num procs per comm on prog\n", world_myrank);
            MPI_Abort(MPI_COMM_WORLD, 0);
        } else
            progid_numcomms_table[i] = progid_numprocs_table[i] / progid_procspercomm_table[i];

    //  - calc number of work comms and starting wrkcomm index of each work comm 
    //    progid_wrkcommctr_table(cpl_numprogids):
    //      starting wrkcomm index of each prog id
    //    cpl_numwrkcomms: 
    //      number of worker communicators
    progid_wrkcommctr_table = (int *)malloc(cpl_numprogids * sizeof(int));
    progid_wrkcommctr_table[0] = 0;
    for (i = 1; i < cpl_numprogids; i++)
        progid_wrkcommctr_table[i] = progid_wrkcommctr_table[i-1] + progid_numcomms_table[i-1];
    cpl_numwrkcomms = progid_wrkcommctr_table[cpl_numprogids-1] + progid_numcomms_table[cpl_numprogids - 1];

    //  - setup map table of work comm id to prog id
    //    cpl_wrkcomm_progid_table(cpl_numwrkcomms): 
    //      table of program id of each workcomm id
    cpl_wrkcomm_progid_table = (int *)malloc(cpl_numwrkcomms * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++)
        for (j = 0; j < progid_numcomms_table[i]; j++) {
            cpl_wrkcomm_progid_table[progid_wrkcommctr_table[i] + j] = progid_table[i];
        }

    //  - setup workcomm id of each rank
    //    rank_wrkcomm_table(0:conn_rowld_nprocs-1): 
    //      table of workcomm id of each rank (-1: requester, cpl_numwrkcomms: coupler, other: worker)
    //    progid_procctr_table(cpl_numprogids): 
    //      table for counting procs of each prog id

    rank_wrkcomm_table = (int *)malloc(world_nprocs * sizeof(int));
    progid_procctr_table = (int *)malloc(cpl_numprogids * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++)
        progid_procctr_table[i] = 0;

    for (i = 0; i < world_nprocs; i++) {
        switch (role_table[i]) {
        case ROLE_WRK:
            p = rank_progid_table[i];
            j = find_progid(progid_table, cpl_numprogids, p);
            rank_wrkcomm_table[i] = progid_wrkcommctr_table[j];
            progid_procctr_table[j]++;
            if (progid_procctr_table[j] == progid_procspercomm_table[j]) {
                progid_procctr_table[j] = 0;
                progid_wrkcommctr_table[j]++;
            }
            break;
        case ROLE_REQ:
            rank_wrkcomm_table[i] = cpl_numwrkcomms+1;
            break;
        case ROLE_CPL:
            rank_wrkcomm_table[i] = cpl_numwrkcomms;
            break;
        }
    }

    //  - setup table of head rank of each workcomm id
    //    cpl_wrkcomm_headrank_table(cpl_numwrkcomms): 
    //      table of headrank of each workcomm id
    //    subrank_table(0:conn_world_nprocs-1)
    //      table of ranks of subcommunicator 
    cpl_wrkcomm_headrank_table = (int *)malloc(cpl_numwrkcomms * sizeof(int));
    MPI_Bcast(rank_wrkcomm_table, world_nprocs, MPI_INT, rank_cpl, MPI_COMM_WORLD);
    MPI_Comm_split(MPI_COMM_WORLD, rank_wrkcomm_table[world_myrank], 0, &CTCA_subcomm);
    MPI_Comm_rank(CTCA_subcomm, &sub_myrank);
    MPI_Allgather(&sub_myrank, 1, MPI_INT, subrank_table, 1, MPI_INT, MPI_COMM_WORLD);
    for (i = 0; i < world_nprocs; i++) 
        if (role_table[i] == ROLE_WRK) 
            if (subrank_table[i] == 0)
                cpl_wrkcomm_headrank_table[rank_wrkcomm_table[i]] = i;

    //  - setup table of status of each workcomm id
    //    cpl_wrkcomm_stat_table(cpl_numwrkcomms): 
    //      table of status of each workcomm id
    cpl_wrkcomm_stat_table = (int *)malloc(cpl_numwrkcomms * sizeof(int));
    for (i = 0; i < cpl_numwrkcomms; i++)
        cpl_wrkcomm_stat_table[i] = WRKSTAT_IDLE;

    //  setup requester id table (used for converting subrank of the requester to world rank)
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            requesterid_table[subrank_table[i]] = i;
    free(rank_progid_table); 
    free(rank_procspercomm_table); 
    free(progid_table); 
    free(progid_numprocs_table); 
    free(progid_procspercomm_table); 
    free(progid_numcomms_table); 
    free(progid_wrkcommctr_table); 
    free(rank_wrkcomm_table); 
    free(progid_procctr_table); 

    return 0;
}

int CTCAC_init()
{
    CTCAC_init_detail(DEF_MAXNUMAREAS, DEF_CPL_NUMREQS, DEF_MAXINTPARAMS, DEF_CPL_DATBUF_SLOTSZ, DEF_CPL_DATBUF_SLOTNUM);

    return 0;
}

int cpl_regarea(int *areaid)
{
    MPI_Aint size_byte;
    int val;
    char *areainfo_item;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    areainfo_item = (char *)areainfo_table + areaidctr * areainfo_itemsize;

    //  Create a window for this area
    size_byte = 0;
    MPI_Win_create(&val, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(MPI_MODE_NOCHECK, areawin_table[areaidctr]);

    //  Get areainfo
    MPI_Bcast(areainfo_item, areainfo_itemsize, MPI_BYTE, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    if (prof_flag == 1) 
        prof_cpl_times[PROF_CPL_REGAREA] += MPI_Wtime() - t0;

    return 0;
}

int CTCAC_regarea_int(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_regarea_real4(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_regarea_real8(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_isfin()
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_isfin() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (mystat == STAT_FIN)
        return 1;
    else
        return 0;
}

int CTCAC_startprof()
{
    if (startprof() < 0) 
        fprintf(stderr, "%d : CTCAC_startprof() : WARNING : prof_flag was not 0\n", world_myrank);
}

int CTCAC_stopprof()
{
    if (stopprof() < 0) 
        fprintf(stderr, "%d : CTCAC_stopprof() : WARNING : prof_flag was not 1\n", world_myrank);
}

void print_reqq(char *msg)
{
    int i;
    char *item;
    for (i = 0; i < cpl_reqq_tail; i++) {
        item = (char *)cpl_reqq + i * cplwrk_req_itemsize;
        fprintf(stderr, " %s : reqq: %d / %d :fromrank %d progid %d entry %d intparamnum %d datasize %lld datbufentry %d\n",
                msg, i, cpl_reqq_tail, CPLWRK_REQ_FROMRANK(item), CPLWRK_REQ_PROGID(item), CPLWRK_REQ_ENTRY(item),
                CPLWRK_REQ_INTPARAMNUM(item), CPLWRK_REQ_DATASIZE(item), CPLWRK_REQ_DATBUFENTRY(item));
    }
}

int cpl_progress()
{
    int i, j, k, progid, bufentry;
    size_t datasize;
    char *reqitem, *reqitem_dest;;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : cpl_progress() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //  find available worker for each request
    for (i = 0; i < cpl_reqq_tail; i++) {
        reqitem = (char *)cpl_reqq + i * cplwrk_req_itemsize;
        progid = CPLWRK_REQ_PROGID(reqitem);
        if (progid >= 0) {
            for (j = 0; j < cpl_numwrkcomms; j++) {
                if (cpl_wrkcomm_progid_table[j] == progid) {
                    if (cpl_wrkcomm_stat_table[j] == WRKSTAT_IDLE) {
                        cpl_wrkcomm_stat_table[j] = WRKSTAT_BUSY;
                        MPI_Send(reqitem, CPLWRK_REQ_SIZE + CPLWRK_REQ_INTPARAMNUM(reqitem) * sizeof(int),
                                 MPI_BYTE, cpl_wrkcomm_headrank_table[j], TAG_REQ, MPI_COMM_WORLD); 
                        datasize = CPLWRK_REQ_DATASIZE(reqitem);
                        if (datasize > 0) {
                            bufentry = CPLWRK_REQ_DATBUFENTRY(reqitem);
                            senddata((char *)cpl_datbuf + bufentry * cpl_datbuf_slotsz, datasize, 
                                     cpl_wrkcomm_headrank_table[j], TAG_DAT, MPI_COMM_WORLD);
                            cpl_datbuf_stat[bufentry] = BUF_AVAIL;
                        }
                        CPLWRK_REQ_PROGID(reqitem) = -1;
                        break;
                    }
                }
            }
        }
    }

    //  clean handled requests
    for (j = 0; j < cpl_reqq_tail; j++) {
        reqitem = (char *)cpl_reqq + j * cplwrk_req_itemsize;
        if (CPLWRK_REQ_PROGID(reqitem) == -1)
            break;
    }
    reqitem_dest = (char *)cpl_reqq + j * cplwrk_req_itemsize;
    for (i = j + 1; i < cpl_reqq_tail; i++) {
        reqitem = (char *)cpl_reqq + i *  cplwrk_req_itemsize;
        if (CPLWRK_REQ_PROGID(reqitem) != -1) {
            memcpy(reqitem_dest, reqitem, cplwrk_req_itemsize);
            j = j + 1;
            reqitem_dest = (char *)cpl_reqq + j * cplwrk_req_itemsize;
        }
    }

    for (i = j; i < cpl_reqq_tail; i++) {
        reqitem = (char *)cpl_reqq + i * cplwrk_req_itemsize;
        CPLWRK_REQ_PROGID(reqitem) = -1;
    }

    cpl_reqq_tail = j;

    return 0;
}

int cpl_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum, void *data, size_t datasz)
{
    MPI_Status stat;
    MPI_Request req;
    int i, reqsize, reqintnum, numpendingwrks, flag, val;
    size_t reqdatasize;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    while (1) {
        cpl_progress();

        numpendingwrks = 0;
        for (i = 0; i < cpl_numwrkcomms; i++)
            if (cpl_wrkcomm_stat_table[i] == WRKSTAT_BUSY)
                numpendingwrks++;

        if ((cpl_runrequesters == 0) && (numpendingwrks == 0) && (cpl_reqq_tail == 0))  {
            for (i = 0; i < cpl_numwrkcomms; i++)
                MPI_Send(&val, 1, MPI_INT, cpl_wrkcomm_headrank_table[i], TAG_FIN, MPI_COMM_WORLD);
            mystat = STAT_FIN;
            break;
        }

        if ((cpl_runrequesters > 0) || (numpendingwrks > 0)) {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stat);

            if (flag) {
                switch (stat.MPI_TAG) {
                case TAG_REQ:
                    MPI_Get_count(&stat, MPI_BYTE, &reqsize);
                    MPI_Recv(cpl_reqbuf, reqsize, MPI_BYTE, stat.MPI_SOURCE, stat.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    reqintnum = (reqsize - REQCPL_REQ_SIZE) / sizeof(int);
                    if (reqintnum != intparamnum) {
                        fprintf(stderr, "%d : cpl_pollreq() : ERROR : inconsistent number of integer parameters\n", world_myrank);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                    }
                    reqdatasize = REQCPL_REQ_DATASIZE(cpl_reqbuf);
                    if (reqdatasize != datasz) {
                        fprintf(stderr, "%d : cpl_pollreq() : ERROR : inconsistent size of data\n", world_myrank);
                        MPI_Abort(MPI_COMM_WORLD, 0);
                    }
                    *fromrank = subrank_table[stat.MPI_SOURCE];
                    CTCAC_reqinfo_set_fromrank(reqinfo, stat.MPI_SOURCE);
                    CTCAC_reqinfo_set_entry(reqinfo, REQCPL_REQ_ENTRY(cpl_reqbuf));
                    CTCAC_reqinfo_set_intparamnum(reqinfo, reqintnum);
                    CTCAC_reqinfo_set_datasize(reqinfo, reqdatasize);
                    memcpy(intparam, (char *)cpl_reqbuf + REQCPL_REQ_SIZE, reqintnum * sizeof(int));
                    if (reqdatasize > 0) 
                        recvdata(data, reqdatasize, stat.MPI_SOURCE, TAG_DAT, MPI_COMM_WORLD);

                    MPI_Send(&val, 1, MPI_INT, stat.MPI_SOURCE, TAG_REP, MPI_COMM_WORLD);
                    return 0;
                case TAG_REP:
                    MPI_Recv(cpl_reqbuf, 1, MPI_INT, stat.MPI_SOURCE, TAG_REP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (i = 0; i < cpl_numwrkcomms; i++)
                        if (cpl_wrkcomm_headrank_table[i] == stat.MPI_SOURCE) {
                            cpl_wrkcomm_stat_table[i] = WRKSTAT_IDLE;
                            break;
                        }
                    break;
                case TAG_FIN:
                    MPI_Get_count(&stat, MPI_INT, &reqintnum);
                    MPI_Recv(cpl_reqbuf, reqintnum, MPI_INT, stat.MPI_SOURCE, TAG_FIN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    cpl_runrequesters--;
                    MPI_Send(&val, 1, MPI_INT, stat.MPI_SOURCE, TAG_REP, MPI_COMM_WORLD);
                    break;
                default:
                    fprintf(stderr, "%d : cpl_pollreq() : ERROR : invalid request tag\n", world_myrank);
                    MPI_Abort(MPI_COMM_WORLD, 0);
                }
            }
        }
    }

    if (prof_flag == 1) 
        prof_cpl_times[PROF_CPL_POLLREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAC_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAC_pollreq_withint(int *reqinfo, int *fromrank, int *intparam, int intparamnum, int *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withint() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAC_pollreq_withreal4(int *reqinfo, int *fromrank, int *intparam, int intparamnum, float *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withreal4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAC_pollreq_withreal8(int *reqinfo, int *fromrank, int *intparam, int intparamnum, double *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withreal8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int cpl_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum, void *data, size_t datasz)
{
    int i, datbufentry;
    char *reqitem;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (cpl_reqq_tail > cpl_maxreqs) {
        fprintf(stderr, "%d : cpl_enqreq() : ERROR : request queue is full\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (intparamnum > maxintparams) {
        fprintf(stderr, "%d : cpl_enqreq() : ERROR : too many parameters\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    reqitem = (char *)cpl_reqq + cpl_reqq_tail * cplwrk_req_itemsize;
    CPLWRK_REQ_FROMRANK(reqitem) = CTCAC_reqinfo_get_fromrank(reqinfo);
    CPLWRK_REQ_PROGID(reqitem) = progid;
    CPLWRK_REQ_ENTRY(reqitem) = CTCAC_reqinfo_get_entry(reqinfo);
    CPLWRK_REQ_INTPARAMNUM(reqitem) = intparamnum;
    CPLWRK_REQ_DATASIZE(reqitem) = datasz;
    memcpy((char *)reqitem + CPLWRK_REQ_SIZE, intparam, intparamnum * sizeof(int));

    if (datasz > 0) {
        if (datasz > cpl_datbuf_slotsz) {
            fprintf(stderr, "%d : cpl_enqreq() : ERROR : data too large %lld %lld\n", world_myrank, datasz, cpl_datbuf_slotsz);
            MPI_Abort(MPI_COMM_WORLD, 0);
        }
        datbufentry = -1;
        for (i = 0; i < cpl_datbuf_slotnum; i++) {
            if (cpl_datbuf_stat[i] == BUF_AVAIL) {
                datbufentry = i;
                cpl_datbuf_stat[i] = BUF_INUSE;
                break;
            }
        }
        if (datbufentry == -1) {
            fprintf(stderr, "%d : cpl_enqreq() : ERROR : data buffer is full\n", world_myrank);
            MPI_Abort(MPI_COMM_WORLD, 0);
        }
        CPLWRK_REQ_DATBUFENTRY(reqitem) = datbufentry;
        memcpy(((char *)cpl_datbuf + datbufentry * cpl_datbuf_slotsz), data, datasz);
    }

    cpl_reqq_tail++;

    if (prof_flag == 1) 
        prof_cpl_times[PROF_CPL_ENQREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAC_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAC_enqreq_withint(int *reqinfo, int progid, int *intparam, int intparamnum, int *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withint() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAC_enqreq_withreal4(int *reqinfo, int progid, int *intparam, int intparamnum, float *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withreal4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAC_enqreq_withreal8(int *reqinfo, int progid, int *intparam, int intparamnum, double *data, size_t datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withreal8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int readarea(int areaid, int reqrank, size_t offset, size_t size, void *dest, int type)
{
    int targetrank;
    char *areainfo_item;
    MPI_Win win;
    MPI_Aint disp;
    MPI_Datatype mpitype;
    int unitsize;
    size_t size_remain, size_toget;
    char *addr;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    areainfo_item = (char *)areainfo_table + areaid * areainfo_itemsize;
    if (AREAINFO_TYPE(areainfo_item, reqrank) != type) {
        fprintf(stderr, "%d : readarea() : ERROR : area type is wrong\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (AREAINFO_SIZE(areainfo_item, reqrank) < size + offset) {
        fprintf(stderr, "%d : readarea() : ERROR : out of range\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    switch (type) {
    case AREA_INT:
        mpitype = MPI_INT;
        unitsize = 4;
        break;
    case AREA_REAL4:
        mpitype = MPI_FLOAT;
        unitsize = 4;
        break;
    case AREA_REAL8:
        mpitype = MPI_DOUBLE;
        unitsize = 8;
        break;
    default:
        fprintf(stderr, "%d : readarea() : ERROR : wrong data type\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    size_remain = size;
    addr = dest;
    while (size_remain > 0) {
        size_toget = ((size_remain * unitsize) > MAX_TRANSFER_SIZE) ? MAX_TRANSFER_SIZE / unitsize : size_remain;
        MPI_Get(addr, size_toget, mpitype, targetrank, disp, size_toget, mpitype, win);
        size_remain -= size_toget;
        addr += size_toget * unitsize;
        disp += size_toget;
    }

    MPI_Win_flush(targetrank, win);

    if (prof_flag == 1) {
        switch(myrole) {
        case ROLE_CPL:
            prof_cpl_times[PROF_CPL_READAREA] += MPI_Wtime() - t0;
            break;
        case ROLE_WRK:
            prof_wrk_times[PROF_WRK_READAREA] += MPI_Wtime() - t0;
            break;
        default:
            fprintf(stderr, "%d : readarea() : ERROR : wrong role %d\n", world_myrank, myrole);
        }
    }

    return 0;
}

int writearea(int areaid, int reqrank, size_t offset, size_t size, void *src, int type)
{
    int targetrank;
    char *areainfo_item;
    MPI_Win win;
    MPI_Aint disp;
    MPI_Datatype mpitype;
    int unitsize;
    size_t size_remain, size_toput;
    char *addr;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    areainfo_item = (char *)areainfo_table + areaid * areainfo_itemsize;
    if (AREAINFO_TYPE(areainfo_item, reqrank) != type) {
        fprintf(stderr, "%d : writearea() : ERROR : area type is wrong\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (AREAINFO_SIZE(areainfo_item, reqrank) < size + offset) {
        fprintf(stderr, "%d : writearea() : ERROR : out of range\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    switch (type) {
    case AREA_INT:
        mpitype = MPI_INT;
        unitsize = 4;
        break;
    case AREA_REAL4:
        mpitype = MPI_FLOAT;
        unitsize = 4;
        break;
    case AREA_REAL8:
        mpitype = MPI_DOUBLE;
        unitsize = 8;
        break;
    default:
        fprintf(stderr, "%d : writearea() : ERROR : wrong data type\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    size_remain = size;
    addr = src;
    while (size_remain > 0) {
        size_toput = ((size_remain * unitsize) > MAX_TRANSFER_SIZE) ? MAX_TRANSFER_SIZE / unitsize : size_remain;
        MPI_Put(addr, size_toput, mpitype, targetrank, disp, size_toput, mpitype, win);
        size_remain -= size_toput;
        addr += size_toput * unitsize;
        disp += size_toput;
    }

    MPI_Win_flush(targetrank, win);


    if (prof_flag == 1) {
        switch(myrole) {
        case ROLE_CPL:
            prof_cpl_times[PROF_CPL_WRITEAREA] += MPI_Wtime() - t0;
            break;
        case ROLE_WRK:
            prof_wrk_times[PROF_WRK_WRITEAREA] += MPI_Wtime() - t0;
            break;
        default:
            fprintf(stderr, "%d : writearea() : ERROR : wrong role %d\n", world_myrank, myrole);
        }
    }

    return 0;
}

int CTCAC_readarea_int(int areaid, int reqrank, size_t offset, size_t size, int *dest)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_INT) < 0) {
        fprintf(stderr, "%d : CTCAC_readarea_int() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_readarea_real4(int areaid, int reqrank, size_t offset, size_t size, float *dest)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_REAL4) < 0) {
        fprintf(stderr, "%d : CTCAC_readarea_real4() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_readarea_real8(int areaid, int reqrank, size_t offset, size_t size, double *dest)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_REAL8) < 0) {
        fprintf(stderr, "%d : CTCAC_readarea_double() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_writearea_int(int areaid, int reqrank, size_t offset, size_t size, int *src)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_writearea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_INT) < 0) {
        fprintf(stderr, "%d : CTCAC_writearea_int() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_writearea_real4(int areaid, int reqrank, size_t offset, size_t size, float *src)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_writearea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_REAL4) < 0) {
        fprintf(stderr, "%d : CTCAC_writearea_real4() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_writearea_real8(int areaid, int reqrank, size_t offset, size_t size, double *src)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_writearea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_REAL8) < 0) {
        fprintf(stderr, "%d : CTCAC_writearea_double() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAC_finalize()
{
    int i;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_finalize() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (prof_print_flag == 1) 
        MPI_Send(prof_cpl_times, PROF_CPL_ITEMNUM, MPI_DOUBLE, requesterid_table[0], TAG_PROF, MPI_COMM_WORLD);

    MPI_Win_unlock_all(win_reqstat);
    MPI_Win_free(&win_reqstat);
    for (i = 0; i < areaidctr; i++) {
        MPI_Win_unlock_all(areawin_table[i]);
        MPI_Win_free(&(areawin_table[i]));
    }

    free_common_tables();
    free(cpl_wrkcomm_progid_table); 
    free(cpl_wrkcomm_headrank_table); 
    free((void *)cpl_wrkcomm_stat_table); 
    free(cpl_reqq); 
    free(cpl_reqbuf); 
    free(cpl_datbuf); 
    free(cpl_datbuf_stat); 

    MPI_Finalize();

    return 0;
}

int CTCAC_prof_start()
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_prof_start() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (startprof() < 0) 
        fprintf(stderr, "%d : CTCAC_prof_start() : WARNING : prof_flag was not 0\n", world_myrank);
}

int CTCAC_prof_stop()
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_prof_stop() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (stopprof() < 0) 
        fprintf(stderr, "%d : CTCAC_prof_stop() : WARNING : prof_flag was not 1\n", world_myrank);
}

int CTCAC_prof_start_calc()
{
    if (myrole != ROLE_CPL) 
        fprintf(stderr, "%d : CTCAC_prof_start_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 0) 
        fprintf(stderr, "%d : CTCAC_prof_start_calc() : WARNING : prof_total_flag was not 0\n", world_myrank);

    prof_calc_flag = 1;

    prof_calc_stime = MPI_Wtime();
}

int CTCAC_prof_stop_calc()
{
    if (myrole != ROLE_CPL) 
        fprintf(stderr, "%d : CTCAC_prof_stop_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 1) 
        fprintf(stderr, "%d : CTCAC_prof_stop_total() : WARNING : prof_total_flag was not 1\n", world_myrank);

    prof_calc_flag = 0;

    prof_cpl_times[PROF_CPL_CALC] += MPI_Wtime() - prof_calc_stime;
}

int CTCAW_init_detail(int progid, int procspercomm, int numareas, int intparams)
{
    int i, sub_myrank;
    int *rank_progid_table, *rank_procspercomm_table, *rank_wrkcomm_table;
    MPI_Aint size_byte;

    myrole = ROLE_WRK;
    maxareas = numareas;
    maxintparams = intparams;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_myrank);

    setup_common_tables(myrole);

    areaidctr = 0;

    //  find rank of coupler
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_CPL) {
            rank_cpl = i;
            break;
        }

    //  attend window creation for request status
    size_byte = 0;
    MPI_Win_create(NULL, size_byte, sizeof(int64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(MPI_MODE_NOCHECK, win_reqstat);

    //  attend gathering information of workers
    rank_progid_table = (int *)malloc(world_nprocs * sizeof(int));
    rank_procspercomm_table = (int *)malloc(world_nprocs * sizeof(int));
    MPI_Allgather(&progid, 1, MPI_INT, rank_progid_table, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&procspercomm, 1, MPI_INT, rank_procspercomm_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  split communicator
    rank_wrkcomm_table = (int *)malloc(world_nprocs * sizeof(int));
    MPI_Bcast(rank_wrkcomm_table, world_nprocs, MPI_INT, rank_cpl, MPI_COMM_WORLD);
    wrk_myworkcomm = rank_wrkcomm_table[world_myrank];
    MPI_Comm_split(MPI_COMM_WORLD, wrk_myworkcomm, 0, &CTCA_subcomm);
    MPI_Comm_rank(CTCA_subcomm, &sub_myrank);
    MPI_Allgather(&sub_myrank, 1, MPI_INT, subrank_table, 1, MPI_INT, MPI_COMM_WORLD);

    //  preapare buffer for incomming request
    cplwrk_req_itemsize = CPLWRK_REQ_SIZE + maxintparams * sizeof(int);
    wrk_reqbuf = (size_t *)malloc(cplwrk_req_itemsize);
    CPLWRK_REQ_FROMRANK(wrk_reqbuf) = -1;
    CPLWRK_REQ_PROGID(wrk_reqbuf) = -1;
    CPLWRK_REQ_ENTRY(wrk_reqbuf) = -1;
    CPLWRK_REQ_INTPARAMNUM(wrk_reqbuf) = -1;
    CPLWRK_REQ_DATASIZE(wrk_reqbuf) = 0;
    CPLWRK_REQ_DATBUFENTRY(wrk_reqbuf) = -1;

    //  setup requester id table (used for converting subrank of the requester to world rank)    
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            requesterid_table[subrank_table[i]] = i;

    mystat = STAT_IDLE;

    free(rank_progid_table); 
    free(rank_procspercomm_table); 
    free(rank_wrkcomm_table); 

    return 0;
}

int CTCAW_init(int progid, int procspercomm)
{
    CTCAW_init_detail(progid, procspercomm, DEF_MAXNUMAREAS, DEF_MAXINTPARAMS);

    return 0;
}

int wrk_regarea(int *areaid)
{
    MPI_Aint size_byte;
    int val;
    char *areainfo_item;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    areainfo_item = (char *)areainfo_table + areaidctr * areainfo_itemsize;

    //  Create a window for this area
    size_byte = 0;
    MPI_Win_create(&val, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(MPI_MODE_NOCHECK, areawin_table[areaidctr]);

    //  Get areainfo
    MPI_Bcast(areainfo_item, areainfo_itemsize, MPI_BYTE, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    if (prof_flag == 1) 
        prof_wrk_times[PROF_WRK_REGAREA] += MPI_Wtime() - t0;

    return 0;
}

int CTCAW_regarea_int(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_regarea_real4(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_regarea_real8(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_isfin()
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_isfin() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (mystat == STAT_FIN)
        return 1;
    else
        return 0;
}

int wrk_pollreq(int *fromrank, int *intparam, int intparamnum, void *data, size_t datasz)
{
    MPI_Status stat;
    int submyrank, subnprocs, i, reqsize, tag, reqintparamnum, val, size_tobcast;
    size_t reqdatasize, size_remain; 
    char *tgt_addr;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (mystat == STAT_FIN) {
        fprintf(stderr, "%d : wrk_pollreq() : ERROR : worker is already in FIN status\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (mystat == STAT_RUNNING) {
        fprintf(stderr, "%d : wrk_pollreq() : ERROR : worker is already in RUNNING status\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Comm_size(CTCA_subcomm, &subnprocs);
    MPI_Comm_rank(CTCA_subcomm, &submyrank);

    if (submyrank == 0) {
        MPI_Probe(rank_cpl, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        switch (stat.MPI_TAG) {
        case TAG_REQ:
            MPI_Get_count(&stat, MPI_BYTE, &reqsize);
            MPI_Recv(wrk_reqbuf, reqsize, MPI_BYTE, stat.MPI_SOURCE, stat.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            reqintparamnum = CPLWRK_REQ_INTPARAMNUM(wrk_reqbuf);
            if (reqintparamnum != intparamnum) {
                fprintf(stderr, "%d : wrk_pollreq() : ERROR : inconsistent number of integer parameters\n", world_myrank);
                MPI_Abort(MPI_COMM_WORLD, 0);
            }
            memcpy(intparam, (char *)wrk_reqbuf + CPLWRK_REQ_SIZE, reqintparamnum * sizeof(int));

            break;
        case TAG_FIN:
            MPI_Recv(&val, 1, MPI_INT, stat.MPI_SOURCE, stat.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            CPLWRK_REQ_PROGID(wrk_reqbuf) = -1; // Flag to finalize
            break;
        default:
            fprintf(stderr, "%d : wrk_pollreq() : ERROR : wrong tag\n", world_myrank);
            MPI_Abort(MPI_COMM_WORLD, 0);
        }
    }

    MPI_Bcast(wrk_reqbuf, CPLWRK_REQ_SIZE + intparamnum * sizeof(int), MPI_BYTE, 0, CTCA_subcomm);

    if (CPLWRK_REQ_PROGID(wrk_reqbuf) != -1) {
        reqdatasize = CPLWRK_REQ_DATASIZE(wrk_reqbuf);
        if (reqdatasize != datasz) {
            fprintf(stderr, "%d : wrk_pollreq() : ERROR : data size is wrong\n", world_myrank);
            MPI_Abort(MPI_COMM_WORLD, 0);
        }
        if (reqdatasize > 0) {
            if (submyrank == 0) 
                recvdata(data, reqdatasize, stat.MPI_SOURCE, TAG_DAT, MPI_COMM_WORLD);

            size_remain = reqdatasize;
            tgt_addr = data;
            while (size_remain > 0) {
                size_tobcast = (size_remain > MAX_TRANSFER_SIZE) ? MAX_TRANSFER_SIZE : size_remain;
                MPI_Bcast(tgt_addr, size_tobcast, MPI_BYTE, 0, CTCA_subcomm);
                tgt_addr += size_tobcast;
                size_remain -= size_tobcast;
            }
        }

        memcpy(intparam, (char *)wrk_reqbuf + CPLWRK_REQ_SIZE, intparamnum * sizeof(int));
        wrk_fromrank = CPLWRK_REQ_FROMRANK(wrk_reqbuf);
        *fromrank = subrank_table[wrk_fromrank];
        wrk_entry = CPLWRK_REQ_ENTRY(wrk_reqbuf);

        mystat = STAT_RUNNING;
    } else 
        mystat = STAT_FIN;

    if (prof_flag == 1) 
        prof_wrk_times[PROF_WRK_POLLREQ] += MPI_Wtime() - t0;

    return 0;
}

int CTCAW_pollreq(int *fromrank, int *intparam, int intparamnum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_pollreq(fromrank, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAW_pollreq_withint(int *fromrank, int *intparam, int intparamnum, int *data, size_t datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withint() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAW_pollreq_withreal4(int *fromrank, int *intparam, int intparamnum, float *data, size_t datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withreal4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAW_pollreq_withreal8(int *fromrank, int *intparam, int intparamnum, double *data, size_t datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withreal8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int CTCAW_complete()
{
    int submyrank, subnprocs, val, res;
    int64_t val64;
    MPI_Aint disp;
    double t0;

    if (prof_flag == 1)
        t0 = MPI_Wtime();

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_complete() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Barrier(CTCA_subcomm);

    if (mystat == STAT_FIN) {
        fprintf(stderr, "%d : CTCAW_complete() : ERROR : worker is already in FIN status\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (mystat == STAT_IDLE) {
        fprintf(stderr, "%d : CTCAW_complete() : ERROR : worker is in IDLE status\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Comm_size(CTCA_subcomm, &subnprocs);
    MPI_Comm_rank(CTCA_subcomm, &submyrank);

    if (submyrank == 0) {
        val = WRKSTAT_IDLE;
        MPI_Send(&val, 1, MPI_INT, rank_cpl, TAG_REP, MPI_COMM_WORLD);
        if (wrk_entry >= 0) {
            val64 = REQSTAT_IDLE;
            disp = wrk_entry;
            MPI_Put((char *)&val64, sizeof(int64_t), MPI_BYTE, wrk_fromrank, disp, sizeof(int64_t), MPI_BYTE, win_reqstat);
            MPI_Win_flush(wrk_fromrank, win_reqstat);
        }

    }

//    MPI_Barrier(CTCA_subcomm);

    mystat = STAT_IDLE;

    if (prof_flag == 1) 
        prof_wrk_times[PROF_WRK_COMPLETE] += MPI_Wtime() - t0;

    return 0;
}

int CTCAW_readarea_int(int areaid, int reqrank, size_t offset, size_t size, int *dest)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_INT) < 0) {
        fprintf(stderr, "%d : CTCAW_readarea_int() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_readarea_real4(int areaid, int reqrank, size_t offset, size_t size, float *dest)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_REAL4) < 0) {
        fprintf(stderr, "%d : CTCAW_readarea_real4() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_readarea_real8(int areaid, int reqrank, size_t offset, size_t size, double *dest)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (readarea(areaid, reqrank, offset, size, dest, AREA_REAL8) < 0) {
        fprintf(stderr, "%d : CTCAW_readarea_double() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_writearea_int(int areaid, int reqrank, size_t offset, size_t size, int *src)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_writearea_int() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_INT) < 0) {
        fprintf(stderr, "%d : CTCAW_writearea_int() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_writearea_real4(int areaid, int reqrank, size_t offset, size_t size, float *src)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_writearea_real4() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_REAL4) < 0) {
        fprintf(stderr, "%d : CTCAW_writearea_real4() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_writearea_real8(int areaid, int reqrank, size_t offset, size_t size, double *src)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_writearea_real8() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (writearea(areaid, reqrank, offset, size, src, AREA_REAL8) < 0) {
        fprintf(stderr, "%d : CTCAW_writearea_double() : ERROR : failed\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    return 0;
}

int CTCAW_finalize()
{
    int i, submyrank, subnprocs;

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_finalize() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (prof_print_flag == 1) {
        MPI_Comm_rank(CTCA_subcomm, &submyrank);
        MPI_Comm_size(CTCA_subcomm, &subnprocs);

//        MPI_Reduce(MPI_IN_PLACE, prof_wrk_times, PROF_WRK_ITEMNUM, MPI_DOUBLE, MPI_MAX, 0, CTCA_subcomm);

        if (submyrank == 0) 
            MPI_Send(prof_wrk_times, PROF_WRK_ITEMNUM, MPI_DOUBLE, requesterid_table[0], TAG_PROF, MPI_COMM_WORLD);
    }
        
    MPI_Win_unlock_all(win_reqstat);
    MPI_Win_free(&win_reqstat);
    for (i = 0; i < areaidctr; i++) {
        MPI_Win_unlock_all(areawin_table[i]);
        MPI_Win_free(&(areawin_table[i]));
    }

    free_common_tables();
    free(wrk_reqbuf); 

    MPI_Finalize();

    return 0;
}

int CTCAW_prof_start()
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_prof_start() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (startprof() < 0) 
        fprintf(stderr, "%d : CTCAW_prof_start() : WARNING : prof_flag was not 0\n", world_myrank);
}

int CTCAW_prof_stop()
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_prof_stop() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    if (stopprof() < 0) 
        fprintf(stderr, "%d : CTCAW_prof_stop() : WARNING : prof_flag was not 1\n", world_myrank);
}

int CTCAW_prof_start_calc()
{
    if (myrole != ROLE_WRK) 
        fprintf(stderr, "%d : CTCAW_prof_start_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 0) 
        fprintf(stderr, "%d : CTCAW_prof_start_calc() : WARNING : prof_total_flag was not 0\n", world_myrank);

    prof_calc_flag = 1;

    prof_calc_stime = MPI_Wtime();
}

int CTCAW_prof_stop_calc()
{
    if (myrole != ROLE_WRK) 
        fprintf(stderr, "%d : CTCAW_prof_stop_calc() : ERROR : wrong role %d\n", world_myrank, myrole);

    if (prof_calc_flag != 1) 
        fprintf(stderr, "%d : CTCAW_prof_stop_total() : WARNING : prof_total_flag was not 1\n", world_myrank);

    prof_calc_flag = 0;

    prof_wrk_times[PROF_WRK_CALC] += MPI_Wtime() - prof_calc_stime;
}



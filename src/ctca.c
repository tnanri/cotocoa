// CoToCoA (Code To Code Adapter)
//  A framework to connect a requester program to multiple worker programs via a coupler program

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

MPI_Comm CTCA_subcomm;

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
#define REQSTAT_IDLE -1
#define WRKSTAT_IDLE 0
#define WRKSTAT_BUSY 1
#define BUF_AVAIL 0
#define BUF_INUSE 1
#define TAG_REQ 10
#define TAG_DAT 20
#define TAG_REP 30
#define TAG_FIN 40
#define AREA_INT 0
#define AREA_REAL4 1
#define AREA_REAL8 2
#define DAT_INT 0
#define DAT_REAL4 1
#define DAT_REAL8 2
#define AREAINFO_ITEMS 2
#define AREAINFO_ITEM_TYPE 0
#define AREAINFO_ITEM_SIZE 1
#define REQCPL_REQITEMS 3
#define REQCPL_REQ_ITEM_ENTRY 0
#define REQCPL_REQ_ITEM_DATTYPE 1
#define REQCPL_REQ_ITEM_DATSZ 2
#define CTCAC_REQINFOITEMS 4
#define CTCAC_REQINFO_ITEM_FROMRANK 0
#define CTCAC_REQINFO_ITEM_ENTRY 1
#define CTCAC_REQINFO_ITEM_INTPARAMNUM 2
#define CTCAC_REQINFO_ITEM_DATSZ 3
#define CPLWRK_REQITEMS 6
#define CPLWRK_REQ_ITEM_FROMRANK 0
#define CPLWRK_REQ_ITEM_PROGID 1
#define CPLWRK_REQ_ITEM_ENTRY 2
#define CPLWRK_REQ_ITEM_INTPARAMNUM 3
#define CPLWRK_REQ_ITEM_DATSZ 4
#define CPLWRK_REQ_ITEM_DATBUFENTRY 5

static int world_myrank, world_nprocs;
static int myrole, mystat, maxareas, rank_cpl, maxintparams;
static int numrequesters, areaidctr;
static int req_maxreqs;
static int64_t req_reqid_ctr;
static int cpl_maxreqs, cpl_numwrkcomms, cpl_runrequesters, cpl_reqq_tail;
static int cpl_datbuf_slotsz, cpl_datbuf_slotnum;
static int wrk_myworkcomm, wrk_fromrank, wrk_entry;
static MPI_Win win_reqstat;
static int *areainfo_table;
static MPI_Win *areawin_table;
static int *areasize_table;
static int *subrank_table;
static int *role_table;
static int *requesterid_table;
static volatile int64_t *req_reqstat_table;
static int *req_reqbuf;
static int *cpl_reqq;
static int *cpl_reqbuf;
static double *cpl_datbuf;
static int *cpl_datbuf_stat;
static int *cpl_wrkcomm_progid_table;
static int *cpl_wrkcomm_headrank_table;
static volatile int *cpl_wrkcomm_stat_table;
static int *wrk_reqbuf;


static int setup_common_tables()
{
    areainfo_table = (int *)malloc(AREAINFO_ITEMS * maxareas * sizeof(int));
    areawin_table = (MPI_Win *)malloc(maxareas * sizeof(MPI_Win));
    role_table = (int *)malloc(world_nprocs * sizeof(int));
    subrank_table = (int *)malloc(world_nprocs * sizeof(int));

    return 0;
}

static int free_common_tables()
{
    free(areainfo_table);
    free(areawin_table);
    free(areasize_table);
    free(role_table);
    free(subrank_table);
    free(requesterid_table);

    return 0;
}

int CTCAR_init_detail(int numareas, int numreqs, int intparams)
{
    int i, sub_myrank, val;
    int *rank_progid_table;
    int *rank_procspercomm_table;
    int *rank_wrkcomm_table;
    MPI_Aint size_byte;

    myrole = ROLE_REQ;
    maxareas = numareas;
    req_maxreqs = numreqs;
    maxintparams = intparams;
    mystat = STAT_RUNNING;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_myrank);

    setup_common_tables();

    areaidctr = 1;

//  gather role of each rank
    MPI_Allgather(&myrole, 1, MPI_INT, role_table, 1, MPI_INT, MPI_COMM_WORLD);

//  count number of requesters
    numrequesters = 0;
    for (i = 0; i < world_nprocs; i++) 
        if (role_table[i] == ROLE_REQ)
            numrequesters++;

//  setup area size table
    areasize_table = (int *)malloc(numrequesters * numareas * sizeof(int));

//  setup request status table
    req_reqstat_table = (int64_t *)malloc(req_maxreqs * sizeof(int64_t));
    for (i = 0; i < req_maxreqs; i++)
        req_reqstat_table[i] = REQSTAT_IDLE;

    size_byte = sizeof(int64_t) * req_maxreqs;
    MPI_Win_create((void *)req_reqstat_table, size_byte, sizeof(int64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(0, win_reqstat);

//  setup a buffer for outgoing request
    req_reqbuf = (int *)malloc((REQCPL_REQITEMS + maxintparams) * sizeof(int));

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
    requesterid_table = (int *)malloc(numrequesters * sizeof(int));
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            requesterid_table[subrank_table[i]] = i;

    free(rank_progid_table);
    free(rank_procspercomm_table);
    free(rank_wrkcomm_table);

    return 0;
}

int CTCAR_init()
{
    return CTCAR_init_detail(DEF_MAXNUMAREAS, DEF_REQ_NUMREQS, DEF_MAXINTPARAMS);
}

int req_regarea(void *base, int size, MPI_Aint size_byte, int unit, int type, int *areaid)
{
//  Create a window for this area
    MPI_Win_create(base, size_byte, unit, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(0, areawin_table[areaidctr]);

//  Set and broadcast areainfo_table
    if (world_myrank == requesterid_table[0]) {
        areainfo_table[areaidctr * AREAINFO_ITEMS + AREAINFO_ITEM_TYPE] = type;
        areainfo_table[areaidctr * AREAINFO_ITEMS + AREAINFO_ITEM_SIZE] = size;
    }
    MPI_Bcast(&(areainfo_table[areaidctr*AREAINFO_ITEMS]), 2, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);

//  Gather sizes to areasize_table of requester 0
    MPI_Gather(&size, 1, MPI_INT, &(areasize_table[areaidctr * numrequesters]), 1, MPI_INT, 0, CTCA_subcomm);

//  Broadcast areasize_table 
    MPI_Bcast(&(areasize_table[areaidctr * numrequesters]), numrequesters, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    return 0;
}

int CTCAR_regarea_int(int *base, int size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_int() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    size_byte = size * sizeof(int);
    return req_regarea((void *)base, size, size_byte, sizeof(int), AREA_INT, areaid);
}

int CTCAR_regarea_real4(float *base, int size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_real4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : () : wrong role\n", world_myrank);
        return -1;
    }
    size_byte = size * sizeof(float);
    return req_regarea((void *)base, size, size_byte, sizeof(float), AREA_REAL4, areaid);
}

int CTCAR_regarea_real8(double *base, int size, int *areaid)
{
    MPI_Aint size_byte;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_regarea_real8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    size_byte = size * sizeof(double);
    return req_regarea((void *)base, size, size_byte, sizeof(double), AREA_REAL8, areaid);
}

int req_sendreq(int *intparams, int numintparams, void *data, int datasize, int datatype, int reqentry)
{
    int reply;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : () : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    if (numintparams > maxintparams) {
        fprintf(stderr, "%d : req_sendreq() : numintparams is too large\n", world_myrank);
    }

//  setup a request message
    req_reqbuf[REQCPL_REQ_ITEM_ENTRY] = reqentry;
    req_reqbuf[REQCPL_REQ_ITEM_DATTYPE] = datatype;
    req_reqbuf[REQCPL_REQ_ITEM_DATSZ] = datasize;
    memcpy(&(req_reqbuf[REQCPL_REQITEMS]), intparams, numintparams * sizeof(int));

//  send a request (with integer data) to the coupler
    MPI_Send(req_reqbuf, REQCPL_REQITEMS + numintparams, MPI_INT, rank_cpl, TAG_REQ, MPI_COMM_WORLD);
    if (datasize > 0) 
        MPI_Send(data, datasize, MPI_BYTE, rank_cpl, TAG_DAT, MPI_COMM_WORLD);

//  wait for a reply from the coupler
    MPI_Recv(&reply, 1, MPI_INT, rank_cpl, TAG_REP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return 0;
}

int CTCAR_sendreq(int *intparams, int numintparams)
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    req_sendreq(intparams, numintparams, NULL, 0, 0, -1);

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

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_hdl() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

//  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
//     couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_hdl() : req_reqstat_table is full\n", world_myrank);
    } else {
//     set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
//     also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
//     increment request id
        req_reqid_ctr++;

//     send a request with this entry
        req_sendreq(intparams, numintparams, NULL, 0, 0, reqentry);
    }

    return 0;
}

int CTCAR_sendreq_withint(int *intparams, int numintparams, int *data, int datanum)
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withint() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(int), DAT_INT, -1);

    return 0;
}

int CTCAR_sendreq_withreal4(int *intparams, int numintparams, float *data, int datanum)
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(float), DAT_REAL4, -1);

    return 0;
}

int CTCAR_sendreq_withreal8(int *intparams, int numintparams, double *data, int datanum)
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    req_sendreq(intparams, numintparams, data, datanum*sizeof(double), DAT_REAL8, -1);

    return 0;
}

int CTCAR_sendreq_withint_hdl(int *intparams, int numintparams, int *data, int datanum, int64_t *hdl)
{
    int reqentry;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withint_hdl() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

//  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
//     couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withint_hdl() : req_reqstat_table is full\n", world_myrank);
    } else {
//     set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
//     also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
//     increment request id
        req_reqid_ctr++;
//     send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(int), DAT_INT, -1);
    }

    return 0;
}

int CTCAR_sendreq_withreal4_hdl(int *intparams, int numintparams, float *data, int datanum, int64_t *hdl)
{
    int reqentry;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4_hdl() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

//  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
//     couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withreal4_hdl() : req_reqstat_table is full\n", world_myrank);
    } else {
//     set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
//     also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
//     increment request id
        req_reqid_ctr++;
//     send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(float), DAT_REAL4, -1);
    }

    return 0;
}

int CTCAR_sendreq_withreal8_hdl(int *intparams, int numintparams, double *data, int datanum, int64_t *hdl)
{
    int reqentry;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8_hdl() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

//  find empty entry of the request status table
    reqentry = find_reqentry();

    if (reqentry == -1) {
//     couldn't find an empty entry
        fprintf(stderr, "%d : CTCAR_sendreq_withreal8_hdl() : req_reqstat_table is full\n", world_myrank);
    } else {
//     set the entry as the current request id
        req_reqstat_table[reqentry] = req_reqid_ctr;
//     also, use the current request id as the handle of this request
        *hdl = req_reqid_ctr;
//     increment request id
        req_reqid_ctr++;
//     send a request with this entry
        req_sendreq(intparams, numintparams, data, datanum*sizeof(double), DAT_REAL8, -1);
    }

    return 0;
}

int CTCAR_wait(int64_t hdl)
{
    int i, flag;

    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_wait() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    do {
        flag = 0;
        for (i = 0; i < req_maxreqs; i++)
            if (req_reqstat_table[i] == hdl) {
                flag = 1;
                break;
            }
    } while (flag == 1);

    return 0;
}

int CTCAR_finalize()
{
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : CTCAR_finalize() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    mystat = STAT_FIN;
    req_reqbuf[0] = -1;
    MPI_Send(req_reqbuf, REQCPL_REQITEMS, MPI_INTEGER, rank_cpl, TAG_FIN, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    free_common_tables();
    free((void *)req_reqstat_table);
    free(req_reqbuf);

    MPI_Finalize();

    return 0;
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
                        break;
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

int CTCAC_init_detail(int numareas, int numreqs, int intparams, int bufslotsz, int bufslotnum)
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

    setup_common_tables();

    areaidctr = 1;

//  gather role of each rank
    MPI_Allgather(&myrole, 1, MPI_INT, role_table, 1, MPI_INT, MPI_COMM_WORLD);
    rank_cpl = world_myrank;

//  count number of requesters
    numrequesters = 0;
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            numrequesters++;
    cpl_runrequesters = numrequesters;

//  setup area size table
    areasize_table = (int *)malloc(numrequesters * numareas * sizeof(int));

//  Attend win_create for request status table on requesters
    size_byte = 0;
    MPI_Win_create(0, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(0, win_reqstat);

//  setup a buffer for incomming requests
    cpl_reqbuf = (int *)malloc((REQCPL_REQITEMS + maxintparams) * sizeof(int));

//  setup a queue for outgoing requests
    cpl_reqq = (int *)malloc((CPLWRK_REQITEMS + maxintparams) * (cpl_maxreqs + 1) * sizeof(int));
    for (i = 0; i < cpl_maxreqs; i++) 
        for (j = 0; j < CPLWRK_REQITEMS; j++)
            cpl_reqq[i * (CPLWRK_REQITEMS + maxintparams) + j] = -1;
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
    free(uniq_progid_table);

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
                    fprintf(stderr, "%d : CTCAC_init_detail() : inconsistent procspercomm of rank\n", world_myrank);
                }
            }
        }
    }

//  - calc number of communicators of each prog id
//    progid_numcomms_table(cpl_numprogids): 
//        number of communicators of each progid
    progid_numcomms_table = (int *)malloc(cpl_numprogids * sizeof(int));
    for (i = 0; i < cpl_numprogids; i++)
        if (progid_numprocs_table[i] % progid_procspercomm_table[i] != 0)
            fprintf(stderr, "%d : CTCAC_init_detail() : num procs per prog id is not dividable by num procs per comm on prog\n", world_myrank);
        else
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
    requesterid_table = (int *)malloc(numrequesters * sizeof(int));
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

//  Create a window for this area
    size_byte = 0;
    MPI_Win_create(&val, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(0, areawin_table[areaidctr]);

//  Get areainfo_table
    MPI_Bcast(&(areainfo_table[areaidctr * AREAINFO_ITEMS]), 2, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);
//  Get areasize_table 
    MPI_Bcast(&(areasize_table[areaidctr * numrequesters]), numrequesters, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    return 0;
}

int CTCAC_regarea_int(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_int() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_regarea_real4(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_real4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_regarea_real8(int *areaid)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_regarea_real8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_regarea(areaid);

    return 0;
}

int CTCAC_isfin()
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_isfin() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    if (mystat == STAT_FIN)
        return 1;
    else
        return 0;
}

int cpl_progress()
{
    int i, j, k, progid, datsz, bufentry, reqentry, reqentry_dest;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : () : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

//  find available worker for each request
    for (i = 0; i < cpl_reqq_tail; i++) {
        reqentry = i * (CPLWRK_REQITEMS + maxintparams);
        progid = cpl_reqq[reqentry + CPLWRK_REQ_ITEM_PROGID];
        for (j = 0; j < cpl_numwrkcomms; j++) {
            if (cpl_wrkcomm_progid_table[j] == progid) {
                if (cpl_wrkcomm_stat_table[j] == WRKSTAT_IDLE) {
                    cpl_wrkcomm_stat_table[j] = WRKSTAT_BUSY;
                    MPI_Send(&(cpl_reqq[reqentry]), 
                             CPLWRK_REQITEMS + cpl_reqq[reqentry + CPLWRK_REQ_ITEM_INTPARAMNUM],
                             MPI_INT, cpl_wrkcomm_headrank_table[j], TAG_REQ, MPI_COMM_WORLD);
                    datsz = cpl_reqq[reqentry + CPLWRK_REQ_ITEM_DATSZ];
                    if (datsz > 0) {
                        bufentry = cpl_reqq[reqentry + CPLWRK_REQ_ITEM_DATBUFENTRY];
                        MPI_Send((char *)cpl_datbuf + bufentry * cpl_datbuf_slotsz, datsz, MPI_BYTE, 
                                 cpl_wrkcomm_headrank_table[j], TAG_DAT, MPI_COMM_WORLD);
                        cpl_datbuf_stat[bufentry] = BUF_AVAIL;
                    }
                    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_PROGID] = -1;
                }
            }
        }
    }

//  clean handled requests
    for (j = 0; j < cpl_reqq_tail; j++)
        if (cpl_reqq[j * (CPLWRK_REQITEMS + maxintparams) + CPLWRK_REQ_ITEM_PROGID] == -1)
            break;
    reqentry_dest = j * (CPLWRK_REQITEMS + maxintparams);
    for (i = j + 1; i < cpl_reqq_tail; i++) {
        reqentry = i * (CPLWRK_REQITEMS + maxintparams);
        if (cpl_reqq[reqentry + CPLWRK_REQ_ITEM_PROGID] != -1) {
            for (k = 0; k < CPLWRK_REQITEMS; k++)
                cpl_reqq[reqentry_dest + k] = cpl_reqq[reqentry + k];
            j = j + 1;
            reqentry_dest = j * (CPLWRK_REQITEMS + maxintparams);
        }
    }
    for (i = j; i < cpl_reqq_tail; i++)
        cpl_reqq[i * (CPLWRK_REQITEMS + maxintparams) + CPLWRK_REQ_ITEM_PROGID] = -1;
    cpl_reqq_tail = j;

    return 0;
}

int cpl_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum, void *data, int datasz)
{
    MPI_Status stat;
    MPI_Request req;
    int i, reqintnum, reqdatasz, numpendingwrks, flag, val;

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
                    MPI_Get_count(&stat, MPI_INT, &reqintnum);
                    MPI_Recv(cpl_reqbuf, reqintnum, MPI_INT, stat.MPI_SOURCE, stat.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    reqintnum -= REQCPL_REQITEMS;
                    if (reqintnum != intparamnum) {
                        fprintf(stderr, "%d : cpl_pollreq() : inconsistent number of integer parameters\n", world_myrank);
                        return -1;
                    }
                    reqdatasz = cpl_reqbuf[REQCPL_REQ_ITEM_DATSZ];
                    if (reqdatasz != datasz) {
                        fprintf(stderr, "%d : cpl_pollreq() : inconsistent size of data\n", world_myrank);
                        return -1;
                    }
                    *fromrank = subrank_table[stat.MPI_SOURCE];
                    if (reqdatasz > 0) 
                        MPI_Irecv(data, reqdatasz, MPI_BYTE, stat.MPI_SOURCE, TAG_DAT, MPI_COMM_WORLD, &req);
                    reqinfo[CTCAC_REQINFO_ITEM_FROMRANK] = stat.MPI_SOURCE;
                    reqinfo[CTCAC_REQINFO_ITEM_ENTRY] = cpl_reqbuf[REQCPL_REQ_ITEM_ENTRY];
                    reqinfo[CTCAC_REQINFO_ITEM_INTPARAMNUM] = reqintnum;
                    reqinfo[CTCAC_REQINFO_ITEM_DATSZ] = reqdatasz;
                    for (i = 0; i < reqintnum; i++)
                        intparam[i] = cpl_reqbuf[REQCPL_REQITEMS + i];
                    if (reqdatasz > 0) 
                        MPI_Wait(&req, MPI_STATUS_IGNORE);
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
                    fprintf(stderr, "%d : cpl_pollreq() : invalid request tag\n", world_myrank);
                    return -1;
                }
            }
        }
    }

    return 0;
}

int CTCAC_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAC_pollreq_withint(int *reqinfo, int *fromrank, int *intparam, int intparamnum, int *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withint() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAC_pollreq_withreal4(int *reqinfo, int *fromrank, int *intparam, int intparamnum, float *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withreal4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAC_pollreq_withreal8(int *reqinfo, int *fromrank, int *intparam, int intparamnum, double *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_pollreq_withreal8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_pollreq(reqinfo, fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int cpl_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum, void *data, int datasz)
{
    int i, datbufentry, reqentry;

    if (cpl_reqq_tail > cpl_maxreqs) {
        fprintf(stderr, "%d : cpl_enqreq() : request queue is full\n", world_myrank);
        return -1;
    }

    if (intparamnum > maxintparams) {
        fprintf(stderr, "%d : cpl_enqreq() : too many parameters\n", world_myrank);
        return -1;
    }

    reqentry = cpl_reqq_tail * (CPLWRK_REQITEMS + maxintparams);
    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_FROMRANK] = reqinfo[CTCAC_REQINFO_ITEM_FROMRANK];
    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_PROGID] = progid;
    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_ENTRY] = reqinfo[CTCAC_REQINFO_ITEM_ENTRY];
    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_INTPARAMNUM] = intparamnum;
    cpl_reqq[reqentry + CPLWRK_REQ_ITEM_DATSZ] = datasz;
    memcpy(&(cpl_reqq[reqentry + CPLWRK_REQITEMS]), intparam, intparamnum * sizeof(int));

    if (datasz > 0) {
        if (datasz > cpl_datbuf_slotsz) {
            fprintf(stderr, "%d : cpl_enqreq() : data too large\n", world_myrank);
            return -1;
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
            fprintf(stderr, "%d : cpl_enqreq() : data buffer is full\n", world_myrank);
            return -1;
        }
        cpl_reqq[reqentry + CPLWRK_REQ_ITEM_DATBUFENTRY] = datbufentry;
        memcpy(((char *)cpl_datbuf + datbufentry * cpl_datbuf_slotsz), data, datasz);
    }

    cpl_reqq_tail++;

    return 0;
}

int CTCAC_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAC_enqreq_withint(int *reqinfo, int progid, int *intparam, int intparamnum, int *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withint() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAC_enqreq_withreal4(int *reqinfo, int progid, int *intparam, int intparamnum, float *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withreal4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAC_enqreq_withreal8(int *reqinfo, int progid, int *intparam, int intparamnum, double *data, int datanum)
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_enqreq_withreal8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    cpl_enqreq(reqinfo, progid, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int CTCAC_readarea_int(int areaid, int reqrank, int offset, int size, int *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_int() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_INT) {
        fprintf(stderr, "%d : CTCAC_readarea_int() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAC_readarea_int() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_INT, targetrank, disp, size, MPI_INT, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAC_readarea_real4(int areaid, int reqrank, int offset, int size, float *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_real4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_REAL4) {
        fprintf(stderr, "%d : CTCAC_readarea_real4() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAC_readarea_real4() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_FLOAT, targetrank, disp, size, MPI_FLOAT, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAC_readarea_real8(int areaid, int reqrank, int offset, int size, double *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_readarea_real8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_REAL8) {
        fprintf(stderr, "%d : CTCAC_readarea_real8() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAC_readarea_real8() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_DOUBLE, targetrank, disp, size, MPI_DOUBLE, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAC_finalize()
{
    if (myrole != ROLE_CPL) {
        fprintf(stderr, "%d : CTCAC_finalize() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    MPI_Barrier(MPI_COMM_WORLD);

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

    setup_common_tables();

    areaidctr = 1;

    MPI_Allgather(&myrole, 1, MPI_INT, role_table, 1, MPI_INT, MPI_COMM_WORLD);

//  count number of requesters
    numrequesters = 0;
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            numrequesters++;

//  setup area size table
    areasize_table = (int *)malloc(numrequesters * numareas * sizeof(int));

//  find rank of coupler
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_CPL) {
            rank_cpl = i;
            break;
        }

//  attend window creation for request status
    size_byte = 0;
    MPI_Win_create(NULL, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(0, win_reqstat);

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
    wrk_reqbuf = (int *)malloc((CPLWRK_REQITEMS + maxintparams) * sizeof(int));
    for (i = 0; i < CPLWRK_REQITEMS; i++)
        wrk_reqbuf[i] = -1;

//  setup requester id table (used for converting subrank of the requester to world rank)    
    requesterid_table = (int *)malloc(numrequesters * sizeof(int));
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

//  Create a window for this area
    size_byte = 0;
    MPI_Win_create(&val, size_byte, 4, MPI_INFO_NULL, MPI_COMM_WORLD, &(areawin_table[areaidctr]));
    MPI_Win_lock_all(0, areawin_table[areaidctr]);

//  Get areainfo_table
    MPI_Bcast(&(areainfo_table[areaidctr * AREAINFO_ITEMS]), 2, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);
//  Get areasize_table 
    MPI_Bcast(&(areasize_table[areaidctr * numrequesters]), numrequesters, MPI_INT, requesterid_table[0], MPI_COMM_WORLD);

    *areaid = areaidctr;
    areaidctr++;

    return 0;
}

int CTCAW_regarea_int(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_int() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_regarea_real4(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_real4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_regarea_real8(int *areaid)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_regarea_real8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_regarea(areaid);

    return 0;
}

int CTCAW_isfin()
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_isfin() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    if (mystat == STAT_FIN)
        return 1;
    else
        return 0;
}

int wrk_pollreq(int *fromrank, int *intparam, int intparamnum, void *data, int datasz)
{
    MPI_Status stat;
    int submyrank, subnprocs, i, bufsz, tag, reqdatasz;

    if (mystat == STAT_FIN) {
        fprintf(stderr, "%d : wrk_pollreq() : worker is already in FIN status\n", world_myrank);
        return -1;
    }

    if (mystat == STAT_RUNNING) {
        fprintf(stderr, "%d : wrk_pollreq() : worker is already in RUNNING status\n", world_myrank);
        return -1;
    }

    MPI_Comm_size(CTCA_subcomm, &subnprocs);
    MPI_Comm_rank(CTCA_subcomm, &submyrank);

    if (submyrank == 0) {
        MPI_Recv(wrk_reqbuf, CPLWRK_REQITEMS + maxintparams, MPI_INT, rank_cpl, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        MPI_Get_count(&stat, MPI_INT, &bufsz);
        tag = stat.MPI_TAG;
        for (i = 1; i < subnprocs; i++)
            MPI_Send(wrk_reqbuf, bufsz, MPI_INT, i, tag, CTCA_subcomm);
    } else {
        MPI_Recv(wrk_reqbuf, CPLWRK_REQITEMS + maxintparams, MPI_INT, 0, MPI_ANY_TAG, CTCA_subcomm, &stat);
        tag = stat.MPI_TAG;
    }

    if (tag == TAG_REQ) {
        reqdatasz = wrk_reqbuf[CPLWRK_REQ_ITEM_DATSZ];
        if (reqdatasz != datasz) {
            fprintf(stderr, "%d : wrk_pollreq() : data size is wrong\n", world_myrank);
            return -1;
        }
        if (datasz > 0) {
            if (submyrank == 0) {
                MPI_Recv(data, datasz, MPI_BYTE, rank_cpl, TAG_DAT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (i = 1; i < subnprocs; i++)
                    MPI_Send(data, datasz, MPI_BYTE, i, TAG_DAT, CTCA_subcomm);
            } else {
                MPI_Recv(data, datasz, MPI_BYTE, 0, TAG_DAT, CTCA_subcomm, MPI_STATUS_IGNORE);
            }
        }
    }

    memcpy(intparam, &(wrk_reqbuf[CPLWRK_REQITEMS]), intparamnum*sizeof(int));
    wrk_fromrank = wrk_reqbuf[CPLWRK_REQ_ITEM_FROMRANK];
    *fromrank = subrank_table[wrk_fromrank];
    wrk_entry = wrk_reqbuf[CPLWRK_REQ_ITEM_ENTRY];

    if (tag == TAG_FIN)
        mystat = STAT_FIN;
    else
        mystat = STAT_RUNNING;

    return 0;
}

int CTCAW_pollreq(int *fromrank, int *intparam, int intparamnum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_pollreq(fromrank, intparam, intparamnum, NULL, 0);

    return 0;
}

int CTCAW_pollreq_withint(int *fromrank, int *intparam, int intparamnum, int *data, int datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withint() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(int));

    return 0;
}

int CTCAW_pollreq_withreal4(int *fromrank, int *intparam, int intparamnum, float *data, int datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withreal4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(float));

    return 0;
}

int CTCAW_pollreq_withreal8(int *fromrank, int *intparam, int intparamnum, double *data, int datanum)
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_pollreq_withreal8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    wrk_pollreq(fromrank, intparam, intparamnum, (void *)data, datanum * sizeof(double));

    return 0;
}

int CTCAW_complete()
{
    int submyrank, subnprocs, val, res;
    MPI_Aint disp;

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_complete() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    MPI_Barrier(CTCA_subcomm);

    if (mystat == STAT_FIN) {
        fprintf(stderr, "%d : CTCAW_complete() : worker is already in FIN status\n", world_myrank);
        return -1;
    }

    if (mystat == STAT_IDLE) {
        fprintf(stderr, "%d : CTCAW_complete() : worker is in IDLE status\n", world_myrank);
        return -1;
    }

    MPI_Comm_size(CTCA_subcomm, &subnprocs);
    MPI_Comm_rank(CTCA_subcomm, &submyrank);

    if (submyrank == 0) {
        if (wrk_entry >= 0) {
            val = REQSTAT_IDLE;
            disp = wrk_entry;
            MPI_Fetch_and_op(&val, &res, MPI_INT, wrk_fromrank, disp, MPI_REPLACE, win_reqstat);
            MPI_Win_flush(wrk_fromrank, win_reqstat);
        }

        val = WRKSTAT_IDLE;
        disp = wrk_myworkcomm;
        MPI_Send(&val, 1, MPI_INT, rank_cpl, TAG_REP, MPI_COMM_WORLD);
    }

    MPI_Barrier(CTCA_subcomm);

    mystat = STAT_IDLE;

    return 0;
}

int CTCAW_readarea_int(int areaid, int reqrank, int offset, int size, int *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_int() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_INT) {
        fprintf(stderr, "%d : CTCAW_readarea_int() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAW_readarea_int() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_INT, targetrank, disp, size, MPI_INT, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAW_readarea_real4(int areaid, int reqrank, int offset, int size, float *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_real4() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_REAL4) {
        fprintf(stderr, "%d : CTCAW_readarea_real4() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAW_readarea_real4() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_FLOAT, targetrank, disp, size, MPI_FLOAT, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAW_readarea_real8(int areaid, int reqrank, int offset, int size, double *dest)
{
    int targetrank, entry;
    MPI_Win win;
    MPI_Aint disp;

    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_readarea_real8() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    entry = AREAINFO_ITEMS * areaid;
    if (areainfo_table[entry + AREAINFO_ITEM_TYPE] != AREA_REAL8) {
        fprintf(stderr, "%d : CTCAW_readarea_real8() : area type is wrong\n", world_myrank);
        return -1;
    }

    if (areainfo_table[entry + AREAINFO_ITEM_SIZE] < size + offset) {
        fprintf(stderr, "%d : CTCAW_readarea_real8() : out of range\n", world_myrank);
        return -1;
    }

    targetrank = requesterid_table[reqrank];
    win = areawin_table[areaid];
    disp = offset;
    MPI_Get(dest, size, MPI_DOUBLE, targetrank, disp, size, MPI_DOUBLE, win);
    MPI_Win_flush(targetrank, win);

    return 0;
}

int CTCAW_finalize()
{
    if (myrole != ROLE_WRK) {
        fprintf(stderr, "%d : CTCAW_finalize() : wrong role %d\n", world_myrank, myrole);
        return -1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    free_common_tables();
    free(wrk_reqbuf);

    MPI_Finalize();

    return 0;
}


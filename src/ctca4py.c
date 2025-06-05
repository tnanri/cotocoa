// CoToCoA (Code To Code Adapter)
//  A framework to connect a requester program to multiple worker programs via a coupler program

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

MPI_Comm CTCA_subcomm;//subcommunicator

#define MAX_TRANSFER_SIZE (1024*1024*1024)

// used to function init_detail
#define DEF_MAXNUMAREAS 10 // Maximum number of areas to expose = 10
#define DEF_REQ_NUMREQS 10// Maximum number of outstanding requests
#define DEF_MAXINTPARAMS 10// Maximum number of elements of the integer array included in a request 

#define DEF_CPL_NUMREQS 10
#define DEF_CPL_DATBUF_SLOTSZ 80000
#define DEF_CPL_DATBUF_SLOTNUM 10

// the role tag of requester,coupler,worker
#define ROLE_REQ 0
#define ROLE_CPL 1
#define ROLE_WRK 2

//indicate the state of each process
#define STAT_FIN 0
#define STAT_IDLE 1
#define STAT_RUNNING 2

#define REQSTAT_IDLE -1LL
//status of each worker communicator
#define WRKSTAT_IDLE 0
#define WRKSTAT_BUSY 1

#define BUF_AVAIL 0//the status of data buffer
#define BUF_INUSE 1
//the tags of sending and recieving
#define TAG_REQ 10 //request
#define TAG_DAT 20 //send data
#define TAG_DATCNT 20//data continue
#define TAG_REP 30 //reply from requester
#define TAG_FIN 40// finish from requester
#define TAG_PROF 50

//
#define AREA_INT 0
#define AREA_REAL4 1
#define AREA_REAL8 2

//indicate the datatype
#define DAT_INT 0//int
#define DAT_REAL4 1//float
#define DAT_REAL8 2//double

#define REQCPL_REQ_OFFSET_ENTRY 0
#define REQCPL_REQ_OFFSET_DATASIZE 4

#define REQCPL_REQ_SIZE 12//the size of request from requester to coupler
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

//locate the address of every data in request from coupler to worker
#define CPLWRK_REQ_FROMRANK(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_FROMRANK)))
#define CPLWRK_REQ_PROGID(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_PROGID)))
#define CPLWRK_REQ_ENTRY(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_ENTRY)))
#define CPLWRK_REQ_INTPARAMNUM(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_INTPARAMNUM)))
#define CPLWRK_REQ_DATASIZE(addr)  (*((size_t *)((char *)(addr) + CPLWRK_REQ_OFFSET_DATASIZE)))
#define CPLWRK_REQ_DATBUFENTRY(addr)  (*((int *)((char *)(addr) + CPLWRK_REQ_OFFSET_DATBUFENTRY)))

static int world_myrank, world_nprocs;//the rank and number of processes in MPI_COMM_WORLD

static int myrole, mystat, maxareas, rank_cpl, maxintparams;

static int numrequesters;//the number of requesters
static int areaidctr;
static int req_maxreqs;//the max number of requester's requests
static int64_t req_reqid_ctr;//

static int req_numwrkgrps;
static int *req_wrkmaster_table;
static int cpl_maxreqs;//the max number of coupler's requests
static int cpl_numwrkcomms;
static int cpl_runrequesters;//the number of the requesters
static int cpl_reqq_tail;//the tail of queue on coupler
static int cpl_datbuf_slotnum;//the number of cpl_datbuf element
static size_t cpl_datbuf_slotsz;
static int wrk_myworkcomm, wrk_fromrank, wrk_entry;
static MPI_Win win_reqstat;

//some main tables
static size_t *areainfo_table;//1
static MPI_Win *areawin_table;//2
static int *subrank_table;//3
static int *role_table;//4 saving the role(0,1,2) of every process
static int *requesterid_table;//5 converting subrank of the requester to world rank
static volatile int64_t *req_reqstat_table;//request status table(will be shared by requestor's processes)
static size_t *req_reqbuf;//buffer for outgoing request 

//the tables and buffers used on coupler
static size_t *cpl_reqq;//the queue (on coupler) for outgoing requests to workers
static size_t *cpl_reqbuf;//a buffer(on coupler) for incoming requests
static double *cpl_datbuf;
static int *cpl_datbuf_stat;
static int *cpl_wrkcomm_progid_table;
static int *cpl_wrkcomm_headrank_table;
static volatile int *cpl_wrkcomm_stat_table;
static size_t *wrk_reqbuf;
static int cpl_reqinfo_entry_mask = (1<<CPL_REQINFO_ENTRY_BITS)-1;//2^12-1
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
/*****************TEST!!!************************/


void test1(){
    printf("this is test1\n");
}

double test2(double x,double y){
    printf("test2\n");
    return x+y;
}

int test3(double *value){
     printf("test3\n");
    *value=*value+2;
    return 0;
}

int test4(double* arr,int size){
    printf("test4\n");
    for(int i=0;i<size;++i){
        arr[i]=i*2.0;
    }
    return 0;
}

MPI_Comm getComm(){
    return CTCA_subcomm;
}

//
MPI_Fint CTCA_get_subcomm(){
    return MPI_Comm_c2f(CTCA_subcomm);
}


/*****************TEST!!!************************/


//create some useful tables: MPI_Win table  for shared memory, role table to save the role of every process(in requester,coupler,worker), subrank table for 
//and areainfo_table for saving the info of the every area, requesterid_table for saving the id of every requester
static int setup_common_tables(int myrole)
{
    int i;

    //create MPI_win table, role table, subrank table
    areawin_table = (MPI_Win *)malloc(maxareas * sizeof(MPI_Win));
    role_table = (int *)malloc(world_nprocs * sizeof(int));
    subrank_table = (int *)malloc(world_nprocs * sizeof(int));

    // gather role of each rank
    //r role of each rankits purpose is to gathe from all processes in a communicator and distribute the gathered data to all processes within that communicator
    MPI_Allgather(&myrole, 1, MPI_INT, role_table, 1, MPI_INT, MPI_COMM_WORLD);

    //count number of requesters
    numrequesters = 0;
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            numrequesters++;
    //
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

//register data to area ID



int req_sendreq(int *intparams, int numintparams, void *data, size_t datasize, int datatype, int reqentry)
{
    int reply;
    //1. examine some limited conditions
    if (myrole != ROLE_REQ) {
        fprintf(stderr, "%d : req_sendreq() : ERROR : wrong role %d\n", world_myrank, myrole);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    
    //maxintparams=10
    if (numintparams > maxintparams) {
        fprintf(stderr, "%d : req_sendreq() : ERROR : numintparams is too large\n", world_myrank);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    //2. setup a request message
    //#define REQCPL_REQ_ENTRY(addr)  (*((int *)((char *)(addr) + REQCPL_REQ_OFFSET_ENTRY)))
    REQCPL_REQ_ENTRY(req_reqbuf) = reqentry;//offset 0
    //#define REQCPL_REQ_DATASIZE(addr)  (*((size_t *)((char *)(addr) + REQCPL_REQ_OFFSET_DATASIZE)))
    REQCPL_REQ_DATASIZE(req_reqbuf) = datasize;//offset 4

    memcpy((char *)req_reqbuf + REQCPL_REQ_SIZE, intparams, numintparams * sizeof(int));

    //3. send a request (with integer data) to the coupler
    MPI_Send(req_reqbuf, REQCPL_REQ_SIZE + numintparams * sizeof(int), MPI_BYTE, rank_cpl, TAG_REQ, MPI_COMM_WORLD);
    if (datasize > 0) 
        senddata(data, datasize, rank_cpl, TAG_DAT, MPI_COMM_WORLD);

    //4. wait for a reply from the coupler
    MPI_Recv(&reply, 1, MPI_INT, rank_cpl, TAG_REP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return 0;
}

// send request from Requester to Coupler


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







//inset and sort
int insert_progid(int *table, int tablesize, int id, int numentries)
{
    int i, j, ret;

    ret = numentries;//0

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

//binary search
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
//get rank from reqinfo
int CTCAC_reqinfo_get_fromrank(int *reqinfo)
{
    return (*reqinfo >> CPL_REQINFO_ENTRY_BITS);
}
//set rank to high bits
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
//set entry to low 12 bit
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

    //printf("after world\n");

    setup_common_tables(myrole);

    areaidctr = 0;

    //1  find rank of coupler
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_CPL) {
            rank_cpl = i;
            break;
        }

    //2 attend window creation for request status
    size_byte = 0;
    MPI_Win_create(NULL, size_byte, sizeof(int64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_reqstat);
    MPI_Win_lock_all(MPI_MODE_NOCHECK, win_reqstat);

    //3  attend gathering information of workers
    rank_progid_table = (int *)malloc(world_nprocs * sizeof(int));
    rank_procspercomm_table = (int *)malloc(world_nprocs * sizeof(int));
    MPI_Allgather(&progid, 1, MPI_INT, rank_progid_table, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&procspercomm, 1, MPI_INT, rank_procspercomm_table, 1, MPI_INT, MPI_COMM_WORLD);

    //4  split communicator
    rank_wrkcomm_table = (int *)malloc(world_nprocs * sizeof(int));
    MPI_Bcast(rank_wrkcomm_table, world_nprocs, MPI_INT, rank_cpl, MPI_COMM_WORLD);
    wrk_myworkcomm = rank_wrkcomm_table[world_myrank];

    MPI_Comm_split(MPI_COMM_WORLD, wrk_myworkcomm, 0, &CTCA_subcomm);
    
    MPI_Comm_rank(CTCA_subcomm, &sub_myrank);
    MPI_Allgather(&sub_myrank, 1, MPI_INT, subrank_table, 1, MPI_INT, MPI_COMM_WORLD);

    //5  preapare buffer for incomming request
    cplwrk_req_itemsize = CPLWRK_REQ_SIZE + maxintparams * sizeof(int);
    wrk_reqbuf = (size_t *)malloc(cplwrk_req_itemsize);
    CPLWRK_REQ_FROMRANK(wrk_reqbuf) = -1;
    CPLWRK_REQ_PROGID(wrk_reqbuf) = -1;
    CPLWRK_REQ_ENTRY(wrk_reqbuf) = -1;
    CPLWRK_REQ_INTPARAMNUM(wrk_reqbuf) = -1;
    CPLWRK_REQ_DATASIZE(wrk_reqbuf) = 0;
    CPLWRK_REQ_DATBUFENTRY(wrk_reqbuf) = -1;

    //6  setup requester id table (used for converting subrank of the requester to world rank)    
    for (i = 0; i < world_nprocs; i++)
        if (role_table[i] == ROLE_REQ)
            requesterid_table[subrank_table[i]] = i;

    mystat = STAT_IDLE;

    free(rank_progid_table); 
    free(rank_procspercomm_table); 
    free(rank_wrkcomm_table);

    //printf("before return\n"); 

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
    //printf("start pollreq\n");
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

//recieve request on Worker
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

// complete request ack on Worker
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



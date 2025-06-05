#include <stdint.h>
#include <stddef.h>
#include <mpi.h>

#define CTCAC_REQINFOITEMS 4
#define CTCA_ROLE_REQ 0
#define CTCA_ROLE_CPL 1
#define CTCA_ROLE_WRK 2

extern MPI_Comm CTCA_subcomm;

/***********just for test *************/
extern void test1();
extern double test2(double x,double y);
extern int test3(double *value);
extern int test4(double* arr,int size);

extern MPI_Comm getComm();

extern MPI_Fint CTCA_get_subcomm();

/*************************************************************************/
/*************************The function about Worker**********************/
extern int CTCAW_init(int progid, int procspercomm);//ok
extern int CTCAW_init_detail(int progid, int procspercomm, int numareas, int intparams);//ok

//Register data to area ID (read the data in Requester from Coupler/Worker like MPI_get)
extern int CTCAW_regarea_int(int *areaid);
extern int CTCAW_regarea_real4(int *areaid);
extern int CTCAW_regarea_real8(int *areaid);

extern int CTCAW_isfin();

//Receive request on Worker
extern int CTCAW_pollreq(int *fromrank, int *intparam, int intparamnum);
extern int CTCAW_pollreq_withint(int *fromrank, int *intparam, int intparamnum, int *data, size_t datanum);
extern int CTCAW_pollreq_withreal4(int *fromrank, int *intparam, int intparamnum, float *data, size_t datanum);
extern int CTCAW_pollreq_withreal8(int *fromrank, int *intparam, int intparamnum, double *data, size_t datanum);

//Complete request ack on Worker
extern int CTCAW_complete();

//Read the data of Requester
extern int CTCAW_readarea_int(int areaid, int reqrank, size_t offset, size_t size, int *dest);
extern int CTCAW_readarea_real4(int areaid, int reqrank, size_t offset, size_t size, float *dest);
extern int CTCAW_readarea_real8(int areaid, int reqrank, size_t offset, size_t size, double *dest);

extern int CTCAW_writearea_int(int areaid, int reqrank, size_t offset, size_t size, int *src);
extern int CTCAW_writearea_real4(int areaid, int reqrank, size_t offset, size_t size, float *src);
extern int CTCAW_writearea_real8(int areaid, int reqrank, size_t offset, size_t size, double *src);

extern int CTCAW_finalize();

//profile
extern int CTCAW_prof_start();
extern int CTCAW_prof_stop();
extern int CTCAW_prof_start_calc();
extern int CTCAW_prof_stop_calc();
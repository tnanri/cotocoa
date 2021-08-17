#include <stdint.h>
#include <stddef.h>
#include <mpi.h>

#define CTCAC_REQINFOITEMS 4
#define CTCA_ROLE_REQ 0
#define CTCA_ROLE_CPL 1
#define CTCA_ROLE_WRK 2

extern MPI_Comm CTCA_subcomm;

extern int CTCAR_init();
extern int CTCAR_init_detail(int numareas, int numreqs, int intparams);
extern int CTCAR_regarea_int(int *base, size_t size, int *areaid);
extern int CTCAR_regarea_real4(float *base, size_t size, int *areaid);
extern int CTCAR_regarea_real8(double *base, size_t size, int *areaid);
extern int CTCAR_sendreq(int *intparams, int numintparams);
extern int CTCAR_sendreq_hdl(int *intparams, int numintparams, int64_t *hdl);
extern int CTCAR_sendreq_withint(int *intparams, int numintparams, int *data, size_t datanum);
extern int CTCAR_sendreq_withreal4(int *intparams, int numintparams, float *data, size_t datanum);
extern int CTCAR_sendreq_withreal8(int *intparams, int numintparams, double *data, size_t datanum);
extern int CTCAR_sendreq_withint_hdl(int *intparams, int numintparams, int *data, size_t datanum, int64_t *hdl);
extern int CTCAR_sendreq_withreal4_hdl(int *intparams, int numintparams, float *data, size_t datanum, int64_t *hdl);
extern int CTCAR_sendreq_withreal8_hdl(int *intparams, int numintparams, double *data, size_t datanum, int64_t *hdl);
extern int CTCAR_prof_start();
extern int CTCAR_prof_stop();
extern int CTCAR_prof_start_total();
extern int CTCAR_prof_stop_total();
extern int CTCAR_prof_start_calc();
extern int CTCAR_prof_stop_calc();
extern int CTCAR_wait(int64_t hdl);
extern int CTCAR_test(int64_t hdl);
extern int CTCAR_finalize();
extern int CTCAC_init();
extern int CTCAC_init_detail(int numareas, int numreqs, int intparams, size_t bufslotsz, int bufslotnum);
extern int CTCAC_regarea_int(int *areaid);
extern int CTCAC_regarea_real4(int *areaid);
extern int CTCAC_regarea_real8(int *areaid);
extern int CTCAC_isfin();
extern int CTCAC_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum);
extern int CTCAC_pollreq_withint(int *reqinfo, int *fromrank, int *intparam, int intparamnum, int *data, size_t datanum);
extern int CTCAC_pollreq_withreal4(int *reqinfo, int *fromrank, int *intparam, int intparamnum, float *data, size_t datanum);
extern int CTCAC_pollreq_withreal8(int *reqinfo, int *fromrank, int *intparam, int intparamnum, double *data, size_t datanum);
extern int CTCAC_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum);
extern int CTCAC_enqreq_withint(int *reqinfo, int progid, int *intparam, int intparamnum, int *data, size_t datanum);
extern int CTCAC_enqreq_withreal4(int *reqinfo, int progid, int *intparam, int intparamnum, float *data, size_t datanum);
extern int CTCAC_enqreq_withreal8(int *reqinfo, int progid, int *intparam, int intparamnum, double *data, size_t datanum);
extern int CTCAC_readarea_int(int areaid, int reqrank, size_t offset, size_t size, int *dest);
extern int CTCAC_readarea_real4(int areaid, int reqrank, size_t offset, size_t size, float *dest);
extern int CTCAC_readarea_real8(int areaid, int reqrank, size_t offset, size_t size, double *dest);
extern int CTCAC_writearea_int(int areaid, int reqrank, size_t offset, size_t size, int *src);
extern int CTCAC_writearea_real4(int areaid, int reqrank, size_t offset, size_t size, float *src);
extern int CTCAC_writearea_real8(int areaid, int reqrank, size_t offset, size_t size, double *src);
extern int CTCAC_finalize();
extern int CTCAC_prof_start();
extern int CTCAC_prof_stop();
extern int CTCAC_prof_start_calc();
extern int CTCAC_prof_stop_calc();
extern int CTCAW_init(int progid, int procspercomm);
extern int CTCAW_init_detail(int progid, int procspercomm, int numareas, int intparams);
extern int CTCAW_regarea_int(int *areaid);
extern int CTCAW_regarea_real4(int *areaid);
extern int CTCAW_regarea_real8(int *areaid);
extern int CTCAW_isfin();
extern int CTCAW_pollreq(int *fromrank, int *intparam, int intparamnum);
extern int CTCAW_pollreq_withint(int *fromrank, int *intparam, int intparamnum, int *data, size_t datanum);
extern int CTCAW_pollreq_withreal4(int *fromrank, int *intparam, int intparamnum, float *data, size_t datanum);
extern int CTCAW_pollreq_withreal8(int *fromrank, int *intparam, int intparamnum, double *data, size_t datanum);
extern int CTCAW_complete();
extern int CTCAW_readarea_int(int areaid, int reqrank, size_t offset, size_t size, int *dest);
extern int CTCAW_readarea_real4(int areaid, int reqrank, size_t offset, size_t size, float *dest);
extern int CTCAW_readarea_real8(int areaid, int reqrank, size_t offset, size_t size, double *dest);
extern int CTCAW_writearea_int(int areaid, int reqrank, size_t offset, size_t size, int *src);
extern int CTCAW_writearea_real4(int areaid, int reqrank, size_t offset, size_t size, float *src);
extern int CTCAW_writearea_real8(int areaid, int reqrank, size_t offset, size_t size, double *src);
extern int CTCAW_finalize();
extern int CTCAW_prof_start();
extern int CTCAW_prof_stop();
extern int CTCAW_prof_start_calc();
extern int CTCAW_prof_stop_calc();

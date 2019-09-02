#include <stdint.h>
#include <mpi.h>

#define CTCAC_REQINFOITEMS 4

extern MPI_Comm CTCA_subcomm;

extern int CTCAR_init();
extern int CTCAR_init_detail(int numareas, int numreqs, int intparams);
extern int CTCAR_regarea_int(int *base, int size, int *areaid);
extern int CTCAR_regarea_real4(float *base, int size, int *areaid);
extern int CTCAR_regarea_real8(double *base, int size, int *areaid);
extern int CTCAR_sendreq(int *intparams, int numintparams);
extern int CTCAR_sendreq_hdl(int *intparams, int numintparams, int64_t *hdl);
extern int CTCAR_sendreq_withint(int *intparams, int numintparams, int *data, int datanum);
extern int CTCAR_sendreq_withreal4(int *intparams, int numintparams, float *data, int datanum);
extern int CTCAR_sendreq_withreal8(int *intparams, int numintparams, double *data, int datanum);
extern int CTCAR_sendreq_withint_hdl(int *intparams, int numintparams, int *data, int datanum, int64_t *hdl);
extern int CTCAR_sendreq_withreal4_hdl(int *intparams, int numintparams, float *data, int datanum, int64_t *hdl);
extern int CTCAR_sendreq_withreal8_hdl(int *intparams, int numintparams, double *data, int datanum, int64_t *hdl);
extern int CTCAR_wait(int64_t hdl);
extern int CTCAR_finalize();
extern int CTCAC_init();
extern int CTCAC_init_detail(int numareas, int numreqs, int intparams, int bufslotsz, int bufslotnum);
extern int CTCAC_regarea_int(int *areaid);
extern int CTCAC_regarea_real4(int *areaid);
extern int CTCAC_regarea_real8(int *areaid);
extern int CTCAC_isfin();
extern int CTCAC_pollreq(int *reqinfo, int *fromrank, int *intparam, int intparamnum);
extern int CTCAC_pollreq_withint(int *reqinfo, int *fromrank, int *intparam, int intparamnum, int *data, int datanum);
extern int CTCAC_pollreq_withreal4(int *reqinfo, int *fromrank, int *intparam, int intparamnum, float *data, int datanum);
extern int CTCAC_pollreq_withreal8(int *reqinfo, int *fromrank, int *intparam, int intparamnum, double *data, int datanum);
extern int CTCAC_enqreq(int *reqinfo, int progid, int *intparam, int intparamnum);
extern int CTCAC_enqreq_withint(int *reqinfo, int progid, int *intparam, int intparamnum, int *data, int datanum);
extern int CTCAC_enqreq_withreal4(int *reqinfo, int progid, int *intparam, int intparamnum, float *data, int datanum);
extern int CTCAC_enqreq_withreal8(int *reqinfo, int progid, int *intparam, int intparamnum, double *data, int datanum);
extern int CTCAC_readarea_int(int areaid, int reqrank, int offset, int size, int *dest);
extern int CTCAC_readarea_real4(int areaid, int reqrank, int offset, int size, float *dest);
extern int CTCAC_readarea_real8(int areaid, int reqrank, int offset, int size, double *dest);
extern int CTCAC_finalize();
extern int CTCAW_init(int progid, int procspercomm);
extern int CTCAW_init_detail(int progid, int procspercomm, int numareas, int intparams);
extern int CTCAW_regarea_int(int *areaid);
extern int CTCAW_regarea_real4(int *areaid);
extern int CTCAW_regarea_real8(int *areaid);
extern int CTCAW_isfin();
extern int CTCAW_pollreq(int *fromrank, int *intparam, int intparamnum);
extern int CTCAW_pollreq_withint(int *fromrank, int *intparam, int intparamnum, int *data, int datanum);
extern int CTCAW_pollreq_withreal4(int *fromrank, int *intparam, int intparamnum, float *data, int datanum);
extern int CTCAW_pollreq_withreal8(int *fromrank, int *intparam, int intparamnum, double *data, int datanum);
extern int CTCAW_complete();
extern int CTCAW_readarea_int(int areaid, int reqrank, int offset, int size, int *dest);
extern int CTCAW_readarea_real4(int areaid, int reqrank, int offset, int size, float *dest);
extern int CTCAW_readarea_real8(int areaid, int reqrank, int offset, int size, double *dest);
extern int CTCAW_finalize();

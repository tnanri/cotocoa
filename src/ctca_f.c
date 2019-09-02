// Intermediate interface for Fortran
#include <ctca.h>
#include <mpi.h>

MPI_Fint CTCA_get_subcomm()
{
    return MPI_Comm_c2f(CTCA_subcomm);
}

MPI_Fint CTCA_get_reqinfoitems()
{
    return CTCAC_REQINFOITEMS;
}

void ctcar_init_f()
{
    CTCAR_init();
}
void ctcar_init_detail_f(int *numareas, int *numreqs, int *intparams)
{
    CTCAR_init_detail(*numareas, *numreqs, *intparams);
}
void ctcar_regarea_int_f(int *base, int *size, int *areaid)
{
    CTCAR_regarea_int(base, *size, areaid);
}
void ctcar_regarea_real4_f(float *base, int *size, int *areaid)
{
    CTCAR_regarea_real4(base, *size, areaid);
}
void ctcar_regarea_real8_f(double *base, int *size, int *areaid)
{
    CTCAR_regarea_real8(base, *size, areaid);
}
void ctcar_sendreq_f(int *intparams, int *numintparams)
{
    CTCAR_sendreq(intparams, *numintparams);
}
void ctcar_sendreq_hdl_f(int *intparams, int *numintparams, int64_t *hdl)
{
    CTCAR_sendreq_hdl(intparams, *numintparams, hdl);
}
void ctcar_sendreq_withint_f(int *intparams, int *numintparams, int *data, int *datanum)
{
    CTCAR_sendreq_withint(intparams, *numintparams, data, *datanum);
}
void ctcar_sendreq_withreal4_f(int *intparams, int *numintparams, float *data, int *datanum)
{
    CTCAR_sendreq_withreal4(intparams, *numintparams, data, *datanum);
}
void ctcar_sendreq_withreal8_f(int *intparams, int *numintparams, double *data, int *datanum)
{
    CTCAR_sendreq_withreal8(intparams, *numintparams, data, *datanum);
}
void ctcar_sendreq_withint_hdl_f(int *intparams, int *numintparams, int *data, int *datanum, int64_t *hdl)
{
    CTCAR_sendreq_withint_hdl(intparams, *numintparams, data, *datanum, hdl);
}
void ctcar_sendreq_withreal4_hdl_f(int *intparams, int *numintparams, float *data, int *datanum, int64_t *hdl)
{
    CTCAR_sendreq_withreal4_hdl(intparams, *numintparams, data, *datanum, hdl);
}
void ctcar_sendreq_withreal8_hdl_f(int *intparams, int *numintparams, double *data, int *datanum, int64_t *hdl)
{
    CTCAR_sendreq_withreal8_hdl(intparams, *numintparams, data, *datanum, hdl);
}
void ctcar_wait_f(int64_t *hdl)
{
    CTCAR_wait(*hdl);
}
void ctcar_finalize_f()
{
    CTCAR_finalize();
}

void ctcac_init_f()
{
    CTCAC_init();
}

void ctcac_init_detail_f(int *numareas, int *numreqs, int *intparams, int *bufslotsz, int *bufslotnum)
{
    CTCAC_init_detail(*numareas, *numreqs, *intparams, *bufslotsz, *bufslotnum);
}

void ctcac_regarea_int_f(int *areaid)
{
    CTCAC_regarea_int(areaid);
}
void ctcac_regarea_real4_f(int *areaid)
{
    CTCAC_regarea_real4(areaid);
}
void ctcac_regarea_real8_f(int *areaid)
{
    CTCAC_regarea_real8(areaid);
}
MPI_Fint ctcac_isfin_f()
{
    return CTCAC_isfin();
}
void ctcac_pollreq_f(int *reqinfo, int *fromrank, int *intparam, int *intparamnum)
{
    CTCAC_pollreq(reqinfo, fromrank, intparam, *intparamnum);
}
void ctcac_pollreq_withint_f(int *reqinfo, int *fromrank, int *intparam, int *intparamnum, int *data, int *datanum)
{
    CTCAC_pollreq_withint(reqinfo, fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcac_pollreq_withreal4_f(int *reqinfo, int *fromrank, int *intparam, int *intparamnum, float *data, int *datanum)
{
    CTCAC_pollreq_withreal4(reqinfo, fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcac_pollreq_withreal8_f(int *reqinfo, int *fromrank, int *intparam, int *intparamnum, double *data, int *datanum)
{
    CTCAC_pollreq_withreal8(reqinfo, fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcac_enqreq_f(int *reqinfo, int *progid, int *intparam, int *intparamnum)
{
    CTCAC_enqreq(reqinfo, *progid, intparam, *intparamnum);
}
void ctcac_enqreq_withint_f(int *reqinfo, int *progid, int *intparam, int *intparamnum, int *data, int *datanum)
{
    CTCAC_enqreq_withint(reqinfo, *progid, intparam, *intparamnum, data, *datanum);
}
void ctcac_enqreq_withreal4_f(int *reqinfo, int *progid, int *intparam, int *intparamnum, float *data, int *datanum)
{
    CTCAC_enqreq_withreal4(reqinfo, *progid, intparam, *intparamnum, data, *datanum);
}
void ctcac_enqreq_withreal8_f(int *reqinfo, int *progid, int *intparam, int *intparamnum, double *data, int *datanum)
{
    CTCAC_enqreq_withreal8(reqinfo, *progid, intparam, *intparamnum, data, *datanum);
}
void ctcac_readarea_int_f(int *areaid, int *reqrank, int *offset, int *size, int *dest)
{
    CTCAC_readarea_int(*areaid, *reqrank, *offset, *size, dest);
}
void ctcac_readarea_real4_f(int *areaid, int *reqrank, int *offset, int *size, float *dest)
{
    CTCAC_readarea_real4(*areaid, *reqrank, *offset, *size, dest);
}
void ctcac_readarea_real8_f(int *areaid, int *reqrank, int *offset, int *size, double *dest)
{
    CTCAC_readarea_real8(*areaid, *reqrank, *offset, *size, dest);
}
void ctcac_finalize_f()
{
    CTCAC_finalize();
}
void ctcaw_init_f(int *progid, int *procspercomm)
{
    CTCAW_init(*progid, *procspercomm);
}
void ctcaw_init_detail_f(int *progid, int *procspercomm, int *numareas, int *intparams)
{
    CTCAW_init_detail(*progid, *procspercomm, *numareas, *intparams);
}
void ctcaw_regarea_int_f(int *areaid)
{
    CTCAW_regarea_int(areaid);
}
void ctcaw_regarea_real4_f(int *areaid)
{
    CTCAW_regarea_real4(areaid);
}
void ctcaw_regarea_real8_f(int *areaid)
{
    CTCAW_regarea_real8(areaid);
}
MPI_Fint ctcaw_isfin_f()
{
    return CTCAW_isfin();
}
void ctcaw_pollreq_f(int *fromrank, int *intparam, int *intparamnum)
{
    CTCAW_pollreq(fromrank, intparam, *intparamnum);
}
void ctcaw_pollreq_withint_f(int *fromrank, int *intparam, int *intparamnum, int *data, int *datanum)
{
    CTCAW_pollreq_withint(fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcaw_pollreq_withreal4_f(int *fromrank, int *intparam, int *intparamnum, float *data, int *datanum)
{
    CTCAW_pollreq_withreal4(fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcaw_pollreq_withreal8_f(int *fromrank, int *intparam, int *intparamnum, double *data, int *datanum)
{
    CTCAW_pollreq_withreal8(fromrank, intparam, *intparamnum, data, *datanum);
}
void ctcaw_complete_f()
{
    CTCAW_complete();
}
void ctcaw_readarea_int_f(int *areaid, int *reqrank, int *offset, int *size, int *dest)
{
    CTCAW_readarea_int(*areaid, *reqrank, *offset, *size, dest);
}
void ctcaw_readarea_real4_f(int *areaid, int *reqrank, int *offset, int *size, float *dest)
{
    CTCAW_readarea_real4(*areaid, *reqrank, *offset, *size, dest);
}
void ctcaw_readarea_real8_f(int *areaid, int *reqrank, int *offset, int *size, double *dest)
{
    CTCAW_readarea_real8(*areaid, *reqrank, *offset, *size, dest);
}
void ctcaw_finalize_f()
{
    CTCAW_finalize();
}


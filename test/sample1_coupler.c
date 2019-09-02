#include <stdio.h>
#include <mpi.h>
#include "ctca.h"

int main()
{
    int myrank, nprocs, progid, fromrank;
    int intparams[2];
    int reqinfo[CTCAC_REQINFOITEMS];
    double data[6*400];
    int i, j, k;
    int prognum = 3;

    printf("coupler init\n");
    CTCAC_init();
    printf("coupler init done\n");

    MPI_Comm_size(CTCA_subcomm, &nprocs);
    MPI_Comm_rank(CTCA_subcomm, &myrank);

    while (1) {
        printf("coupler poll\n");
        CTCAC_pollreq_withreal8(reqinfo, &fromrank, intparams, 2, data, 6*400);
        printf("coupler poll done\n");

        if (CTCAC_isfin()) 
            break;

        if (fromrank >= 0) {
        printf("coupler enq\n");
            progid = intparams[0];
            CTCAC_enqreq_withreal8(reqinfo, progid, intparams, 2, data, 6*400);
        printf("coupler enq done\n");
        }
    }

    printf("coupler fin\n");

    fprintf(stderr, "%d: coupler finalize\n", myrank);
    CTCAC_finalize();
}

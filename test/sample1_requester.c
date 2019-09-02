#include <stdio.h>
#include <mpi.h>
#include "ctca.h"

int main()
{
    int myrank, nprocs, progid;
    int intparams[2];
    double data[6*400];
    int i, j, k;
    int prognum = 3;
    
    printf("requester init\n");
    CTCAR_init();
    printf("requester init done\n");

    MPI_Comm_size(CTCA_subcomm, &nprocs);
    MPI_Comm_rank(CTCA_subcomm, &myrank);

    for (i = 0; i < 10; i++) {
        progid = i % prognum;
        intparams[0] = progid;
        intparams[1] = i;

        for (j = 0; j < 400; j++)
            for (k = 0; k < 6; k++)
                data[j*6 + k] = i*10000 + j*10 + k;

        if (myrank == 0) 
            CTCAR_sendreq_withreal8(intparams, 2, data, 6*400);

    }

    fprintf(stderr, "%d: requester finalize\n", myrank);
    CTCAR_finalize();
}

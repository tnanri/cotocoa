#include <stdio.h>
#include <mpi.h>
#include "ctca.h"

int main()
{
    int myrank, nprocs, progid, fromrank;
    int intparams[2];
    double data[6*400];
    int i, j, k, c;
    int prognum = 3;

    printf("worker2 init\n");
    CTCAW_init(1, 4);
    printf("worker2 init done\n");

    MPI_Comm_size(CTCA_subcomm, &nprocs);
    MPI_Comm_rank(CTCA_subcomm, &myrank);

    while(1) {
        CTCAW_pollreq_withreal8(&fromrank, intparams, 2, data, 6*400);
        c = intparams[1];

        if (CTCAW_isfin()) 
            break;

        printf("worker2 poll req done\n");

        MPI_Bcast(data, 6*400, MPI_DOUBLE, 0, CTCA_subcomm);

        printf("worker2 bcast done\n");

        printf("worker2 rank %d c %d data %f %f %f %f %f\n", myrank, c, data[0], data[1], data[2], data[3], data[4]);
        
        CTCAW_complete();
        printf("worker2 complete done\n");
    }

    fprintf(stderr, "%d: worker2 finalize\n", myrank);
    CTCAW_finalize();
}

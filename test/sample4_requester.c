#include <stdio.h>
#include <mpi.h>
#include "ctca.h"
#include <stdlib.h>
#include <time.h>
#define MAX_RANDOM_OFFSET 1000
#define TEST_SIZE 10000/8
int main()
{

    int myrank, nprocs, progid;

    int intparams[2];

    double data[TEST_SIZE];
    int i, j, k;
    //int prognum = 3;
    int prognum = 1; //for test
    
    srand(time(NULL));
    int offset = rand() % MAX_RANDOM_OFFSET;
    printf("random offset: %d \n",offset);
    
    printf("requester init\n");
    CTCAR_init();
    printf("requester init done\n");

    MPI_Comm_size(CTCA_subcomm, &nprocs);
    MPI_Comm_rank(CTCA_subcomm, &myrank);

    CTCAR_prof_start();
    CTCAR_prof_start_total();
    CTCAR_prof_start_calc();

    for (i = 0; i < 10; i++) {
        progid = i % prognum;
        intparams[0] = progid;
        intparams[1] = i;

        for(j=0;j<TEST_SIZE;j++){
            data[j]=(double)j+offset;
        }
        if (myrank == 0)
            CTCAR_sendreq_withreal8(intparams, 2, data, TEST_SIZE);
    }

    CTCAR_prof_stop_calc();
    CTCAR_prof_stop_total();
    CTCAR_prof_stop();

    fprintf(stderr, "%d: requester finalize\n", myrank);
    CTCAR_finalize();
}

#include <stdio.h>
#include <mpi.h>
#include "ctca.h"
#define TEST_SIZE 10000/8
int main()
{
    int myrank, nprocs, progid, fromrank;
    int intparams[2];
    int reqinfo[CTCAC_REQINFOITEMS];
    double data[TEST_SIZE];
    int i, j, k;
    
    //int prognum = 3;
    
    printf("coupler init\n");
    CTCAC_init();
    printf("coupler init done\n");

    MPI_Comm_size(CTCA_subcomm, &nprocs);
    MPI_Comm_rank(CTCA_subcomm, &myrank);
    CTCAC_prof_start();
    CTCAC_prof_start_calc();

    while (1) {
        printf("coupler poll\n");
        CTCAC_pollreq_withreal8(reqinfo, &fromrank, intparams, 2, data, TEST_SIZE);
        printf("coupler poll done\n");

        if (CTCAC_isfin()){
            break;

        }
        
        if (fromrank >= 0) {
            printf("coupler enq\n");
            progid = intparams[0];
            CTCAC_enqreq_withreal8(reqinfo, progid, intparams, 2, data, TEST_SIZE);
            printf("coupler enq done\n");
        }
    }

    CTCAC_prof_stop_calc();
    CTCAC_prof_stop();
    printf("coupler fin\n");

    fprintf(stderr, "%d: coupler finalize\n", myrank);
    CTCAC_finalize();
}
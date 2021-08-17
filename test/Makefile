PREFIX=/usr/local/cotocoa
CC=mpicc
FC=mpifort
CFLAGS=-I${PREFIX}/include
LDFLAGS=-L${PREFIX}/lib
LIBS=-lctca
F_LIBS=-lctca_f -lctca_mod

PROGS = sample1_requester sample1_coupler sample1_worker1 sample1_worker2 sample1_worker3 \
        sample2_requester sample2_coupler sample2_worker1 sample2_worker2 sample2_worker3 \
		sample3_requester sample3_coupler sample3_worker

all: ${PROGS}

sample1_requester: sample1_requester.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_requester.c $(LIBS) -o sample1_requester

sample1_coupler: sample1_coupler.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_coupler.c $(LIBS) -o sample1_coupler

sample1_worker1: sample1_worker1.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker1.c $(LIBS) -o sample1_worker1

sample1_worker2: sample1_worker2.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker2.c $(LIBS) -o sample1_worker2

sample1_worker3: sample1_worker3.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker3.c $(LIBS) -o sample1_worker3

sample2_requester: sample2_requester.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_requester.f90 $(F_LIBS) $(LIBS) -o sample2_requester

sample2_coupler: sample2_coupler.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_coupler.f90 $(F_LIBS) $(LIBS) -o sample2_coupler

sample2_worker1: sample2_worker1.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker1.f90 $(F_LIBS) $(LIBS) -o sample2_worker1

sample2_worker2: sample2_worker2.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker2.f90 $(F_LIBS) $(LIBS) -o sample2_worker2

sample2_worker3: sample2_worker3.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker3.f90 $(F_LIBS) $(LIBS) -o sample2_worker3

sample3_requester: sample3_requester.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample3_requester.f90 $(F_LIBS) $(LIBS) -o sample3_requester

sample3_coupler: sample3_coupler.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample3_coupler.f90 $(F_LIBS) $(LIBS) -o sample3_coupler

sample3_worker: sample3_worker.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample3_worker.f90 $(F_LIBS) $(LIBS) -o sample3_worker

clean:
	/bin/rm -f *.o *.mod ${PROGS}

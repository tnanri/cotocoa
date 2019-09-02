PREFIX=/home/usr3/k70043a/local/ctca-1.0
CC=mpicc
FC=mpif90 
CFLAGS=-I${PREFIX}/include
LDFLAGS=-L${PREFIX}/lib
LIBS=-lctca
F_LIBS=-lctca_f -lctca_mod

PROGS = sample1_requester_c sample1_coupler_c sample1_worker1_c sample1_worker2_c sample1_worker3_c \
        sample1_requester_f sample1_coupler_f sample1_worker1_f sample1_worker2_f sample1_worker3_f \
        sample2_requester_f sample2_coupler_f sample2_worker1_f sample2_worker2_f sample2_worker3_f

all: ${PROGS}

sample1_requester_c: sample1_requester.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_requester.c $(LIBS) -o sample1_requester_c

sample1_coupler_c: sample1_coupler.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_coupler.c $(LIBS) -o sample1_coupler_c

sample1_worker1_c: sample1_worker1.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker1.c $(LIBS) -o sample1_worker1_c

sample1_worker2_c: sample1_worker2.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker2.c $(LIBS) -o sample1_worker2_c

sample1_worker3_c: sample1_worker3.c
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker3.c $(LIBS) -o sample1_worker3_c

sample1_requester_f: sample1_requester.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_requester.f90 $(F_LIBS) $(LIBS) -o sample1_requester_f

sample1_coupler_f: sample1_coupler.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_coupler.f90 $(F_LIBS) $(LIBS) -o sample1_coupler_f

sample1_worker1_f: sample1_worker1.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker1.f90 $(F_LIBS) $(LIBS) -o sample1_worker1_f

sample1_worker2_f: sample1_worker2.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker2.f90 $(F_LIBS) $(LIBS) -o sample1_worker2_f

sample1_worker3_f: sample1_worker3.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker3.f90 $(F_LIBS) $(LIBS) -o sample1_worker3_f

sample2_requester_f: sample2_requester.f90
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_requester.f90 $(F_LIBS) $(LIBS) -o sample2_requester_f

sample2_coupler_f: sample2_coupler.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_coupler.f90 $(F_LIBS) $(LIBS) -o sample2_coupler_f

sample2_worker1_f: sample2_worker1.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker1.f90 $(F_LIBS) $(LIBS) -o sample2_worker1_f

sample2_worker2_f: sample2_worker2.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker2.f90 $(F_LIBS) $(LIBS) -o sample2_worker2_f

sample2_worker3_f: sample2_worker3.f90 
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker3.f90 $(F_LIBS) $(LIBS) -o sample2_worker3_f

clean:
	/bin/rm -f *.o *.mod ${PROGS}

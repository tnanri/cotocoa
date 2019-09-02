PREFIX=/usr/local/ctca-1.0
CC=mpicc
FC=mpif90 
CFLAGS=-I.
LDFLAGS=-L.
LIBS=-lctca
F_LIBS=-lctca_f -lctca_mod
#FC=/home/usr3/k70043a/local/openmpi-4.0.0-noucx/bin/mpif90

TARGETS=libctca.a libctca_f.a libctca_mod.a

all: ${TARGETS}

libctca.a:	ctca.o
	ar rc libctca.a ctca.o

libctca_f.a: ctca_f.o
	ar rc libctca_f.a ctca_f.o

libctca_mod.a: ctca_mod.o
	ar rc libctca_mod.a ctca_mod.o

sample1_requester_c: sample1_requester.c libctca.a
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_requester.c $(LIBS) -o sample1_requester_c

sample1_coupler_c: sample1_coupler.c libctca.a
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_coupler.c $(LIBS) -o sample1_coupler_c

sample1_worker1_c: sample1_worker1.c libctca.a
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker1.c $(LIBS) -o sample1_worker1_c

sample1_worker2_c: sample1_worker2.c libctca.a
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker2.c $(LIBS) -o sample1_worker2_c

sample1_worker3_c: sample1_worker3.c libctca.a
	$(CC) $(CFLAGS) $(LDFLAGS) sample1_worker3.c $(LIBS) -o sample1_worker3_c

sample1_requester_f: sample1_requester.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_requester.f90 $(F_LIBS) $(LIBS) -o sample1_requester_f

sample1_coupler_f: sample1_coupler.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_coupler.f90 $(F_LIBS) $(LIBS) -o sample1_coupler_f

sample1_worker1_f: sample1_worker1.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker1.f90 $(F_LIBS) $(LIBS) -o sample1_worker1_f

sample1_worker2_f: sample1_worker2.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker2.f90 $(F_LIBS) $(LIBS) -o sample1_worker2_f

sample1_worker3_f: sample1_worker3.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample1_worker3.f90 $(F_LIBS) $(LIBS) -o sample1_worker3_f

sample2_requester_f: sample2_requester.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_requester.f90 $(F_LIBS) $(LIBS) -o sample2_requester_f

sample2_coupler_f: sample2_coupler.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_coupler.f90 $(F_LIBS) $(LIBS) -o sample2_coupler_f

sample2_worker1_f: sample2_worker1.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker1.f90 $(F_LIBS) $(LIBS) -o sample2_worker1_f

sample2_worker2_f: sample2_worker2.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker2.f90 $(F_LIBS) $(LIBS) -o sample2_worker2_f

sample2_worker3_f: sample2_worker3.f90 libctca.a libctca_f.a libctca_mod.a
	$(FC) $(CFLAGS) $(LDFLAGS) sample2_worker3.f90 $(F_LIBS) $(LIBS) -o sample2_worker3_f

ctca.o: ctca.c
	$(CC) $(CFLAGS) -c ctca.c

ctca_f.o: ctca_f.c
	$(CC) $(CFLAGS) -c ctca_f.c

ctca_mod.o: ctca_mod.f90
	$(FC) $(CFLAGS) -c ctca_mod.f90

install: all
	/usr/bin/mkdir -p ${PREFIX}/lib
	/usr/bin/mkdir -p ${PREFIX}/include
	/bin/cp libctca.a libctca_f.a libctca_mod.a ${PREFIX}/lib
	/bin/cp ctca.h ctca.mod ${PREFIX}/include

clean:
	/bin/rm -f *.o *.mod ${TARGETS}

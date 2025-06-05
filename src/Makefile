PREFIX=${HOME}/local/cotocoa/1.3.1-test
CC=mpiicc
FC=mpiifort
CFLAGS=-I.
LDFLAGS=-L.
LIBS=-lctca
F_LIBS=-lctca_f -lctca_mod

TARGETS=libctca.a libctca_f.a libctca_mod.a ctca4py.so

all: ${TARGETS}

libctca.a:	ctca.o
	ar rc libctca.a ctca.o

libctca_f.a: ctca_f.o
	ar rc libctca_f.a ctca_f.o

libctca_mod.a: ctca_mod.o
	ar rc libctca_mod.a ctca_mod.o

ctca.o: ctca.c
	$(CC) $(CFLAGS) -c ctca.c

ctca_f.o: ctca_f.c
	$(CC) $(CFLAGS) -c ctca_f.c

ctca_mod.o: ctca_mod.f90
	$(FC) $(CFLAGS) -c ctca_mod.f90

ctca4py.so: ctca4py.c
	$(CC) -fpic -c ctca4py.c
	$(CC) -shared -o ctca4py.so ctca4py.o

install: all
	/usr/bin/mkdir -p ${PREFIX}/lib/ctca4py
	/usr/bin/mkdir -p ${PREFIX}/include
	/bin/cp ${TARGETS} ${PREFIX}/lib
	/usr/bin/sed "s+INSTALLLIBDIR+${PREFIX}/lib+g" ctcaworker.py > ${PREFIX}/lib/ctca4py/ctcaworker.py
	/usr/bin/touch ${PREFIX}/lib/ctca4py/__init__.py
	/bin/cp ctca.h ctca.mod ${PREFIX}/include

clean:
	/bin/rm -f *.o *.mod ${TARGETS}

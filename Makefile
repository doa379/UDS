LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =

SRC_LIBSQUEUE = squeue.c
OBJ_LIBSQUEUE = ${SRC_LIBSQUEUE:.c=.o}
SRC_TESTSQUEUE = test_squeue.c
OBJ_TESTSQUEUE = ${SRC_TESTSQUEUE:.c=.o}

SRC_LIBSHM = shm.c
OBJ_LIBSHM = ${SRC_LIBSHM:.c=.o}
SRC_TESTSHM = test_shm.c
OBJ_TESTSHM = ${SRC_TESTSHM:.c=.o}

SRC_LIBPQUEUE = pqueue.c
OBJ_LIBPQUEUE = ${SRC_LIBPQUEUE:.c=.o}
SRC_TESTPQUEUE = test_pqueue.c
OBJ_TESTPQUEUE = ${SRC_TESTPQUEUE:.c=.o}

CC = gcc
CFLAGS = -std=c99 -c -g -Wall -Werror -pie -fPIC ${INCS}

all: libsqueue.so test_squeue libshm.so test_shm libpqueue.so test_pqueue

.c.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libsqueue.so: ${OBJ_LIBSQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBSQUEUE} -l pthread

test_squeue: ${OBJ_TESTSQUEUE}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TESTSQUEUE} -L $(CURDIR) -l squeue -Wl,-rpath,$(CURDIR)

libshm.so: ${OBJ_LIBSHM}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBSHM}

test_shm: ${OBJ_TESTSHM}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TESTSHM} -L $(CURDIR) -l shm -Wl,-rpath,$(CURDIR)

libpqueue.so: ${OBJ_LIBPQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBPQUEUE}

test_pqueue: ${OBJ_TESTPQUEUE}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TESTPQUEUE} -L $(CURDIR) -l pqueue -Wl,-rpath,$(CURDIR)

clean:
		@echo Cleaning
		@rm -f ${OBJ_LIBSQUEUE} ${OBJ_TESTSQUEUE} ${OBJ_LIBSHM} ${OBJ_TESTSHM} ${OBJ_LIBPQUEUE} ${OBJ_TESTPQUEUE}
		@rm -f test_squeue test_shm test_pqueue


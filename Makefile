LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =

SRC_LIBSQUEUE = squeue.c
OBJ_LIBSQUEUE = ${SRC_LIBSQUEUE:.c=.o}
SRC_TESTSQUEUE = test_squeue.c
OBJ_TESTSQUEUE = ${SRC_TESTSQUEUE:.c=.o}

SRC_LIBPQUEUE = pqueue.c
OBJ_LIBPQUEUE = ${SRC_LIBPQUEUE:.c=.o}
SRC_TESTPQUEUE = test_pqueue.c
OBJ_TESTPQUEUE = ${SRC_TESTPQUEUE:.c=.o}

CC = gcc
CFLAGS = -std=c99 -c -g -Wall -Werror -pie -fPIC ${INCS}

all: libsqueue.so test_squeue libpqueue.so test_pqueue

.c.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libsqueue.so: ${OBJ_LIBSQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBSQUEUE} -l pthread

test_squeue: ${OBJ_TESTSQUEUE}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TESTSQUEUE} -L $(CURDIR) -l squeue -Wl,-rpath,$(CURDIR)

libpqueue.so: ${OBJ_LIBPQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBPQUEUE}

test_pqueue: ${OBJ_TESTPQUEUE}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TESTPQUEUE} -L $(CURDIR) -l pqueue -Wl,-rpath,$(CURDIR)

clean:
		@echo Cleaning
		@rm -f ${OBJ_LIBSQUEUE} ${OBJ_TESTSQUEUE} ${OBJ_LIBPQUEUE} ${OBJ_TESTPQUEUE}
		@rm -f test_squeue test_pqueue


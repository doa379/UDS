LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =
LIBS = -l pthread

SRC_LIBQUEUE = squeue.c
OBJ_LIBQUEUE = ${SRC_LIBQUEUE:.c=.o}

SRC_TEST = test_squeue.c
OBJ_TEST = ${SRC_TEST:.c=.o}

CC = gcc
CFLAGS = -std=c99 -c -g -Wall -Werror -pie -fPIC ${INCS}
LDFLAGS = ${LIBS}

all: libsqueue.so test_squeue

.c.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libsqueue.so: ${OBJ_LIBQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBQUEUE} ${LDFLAGS}

test_squeue: ${OBJ_TEST}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TEST} ${LDFLAGS} -L $(CURDIR) -l squeue -Wl,-rpath,$(CURDIR)

clean:
		@echo Cleaning
		@rm -f ${OBJ_LIBQUEUE} ${OBJ_TEST}
		@rm -f test_squeue


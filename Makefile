SRC = test-shm.c test-pipe.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
TARGET = $(basename $(OBJ))
CC = gcc
CCFLAGS = -Wall

all: $(TARGET)

test-shm: test-shm.o util.o
	${CC} ${CCFLAGS} -pthread -o $@ test-shm.o util.o

test-pipe: test-pipe.o util.o
	${CC} ${CCFLAGS} -o $@ test-pipe.o util.o

util.o: util.c util.h
	${CC} ${CCFLAGS} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o $(TARGET)

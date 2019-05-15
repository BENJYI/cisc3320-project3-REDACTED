SRC = test-shm.c test-pipe.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
TARGET = $(basename $(OBJ))
CC = gcc
CCFLAGS = -Wall

all: $(TARGET)

test-single: test-single.o util.o
	${CC} ${CCFLAGS} -o $@ test-single.o util.o

test-pipe: test-shm.o util.o
	${CC} ${CCFLAGS} -o $@ test-shm.o util.o

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o $(TARGET)

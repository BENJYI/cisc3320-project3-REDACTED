SRC = test-shm.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
TARGET = $(basename $(OBJ))
CC = gcc
CCFLAGS = -Wall

all: $(TARGET)

test-shm: test-shm.o util.o
	${CC} ${CCFLAGS} -o $@ test-shm.o

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o $(TARGET)

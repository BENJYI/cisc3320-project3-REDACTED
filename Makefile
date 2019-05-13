SRC = test-single.c test-switch.c test-switch2.c test-pthread.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
TARGET = $(basename $(OBJ))
CC = gcc
CCFLAGS = -Wall

all: $(TARGET)

main: main.o util.o
	${CC} ${CCFLAGS} -o $@ main.o

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o $(TARGET)

CC  = g++ 
INCLUDES = ./includes
LIBS =
CFLAGS = -I${INCLUDES} -O0 -g3 -Wall -std=c++0x
OBJS = 
HEADS = unp.h config.h
TARGET = byteorder

vpath %.c ./src
vpath %.c ./libs
vpath %.h ${INCLUDES}
vpath %.cpp ./src
vpath %.cpp ./libs
vpath %.hpp ${INCLUDES}


all:	${TARGET}

byteorder: byteorder.cpp ${OBJS} ${HEAD}
		${CC} ${CFLAGS} -o $@ ./src/byteorder.cpp ${OBJS} ${LIBS}

		
.PHONY: clean
clean:
		rm ${TARGET} ${OBJS} -rf




CC  = g++ 
INCLUDES = ./includes
LIBS =
CFLAGS = -I${INCLUDES} -O0 -g3 -Wall -std=c++0x
OBJS = error.o wrapsock.o wrapunix.o wrapstdio.o wraplib.o
HEADS = unp.h config.h
TARGET = daytimetcpsrv daytimetcpcli

vpath %.c ./src
vpath %.c ./libs
vpath %.h ${INCLUDES}
vpath %.cpp ./src
vpath %.cpp ./libs
vpath %.hpp ${INCLUDES}


all:	${TARGET}

daytimetcpsrv: daytimetcpsrv.cpp ${OBJS} ${HEAD}
		${CC} ${CFLAGS} -o $@ ./src/daytimetcpsrv.cpp ${OBJS} ${LIBS}

daytimetcpcli: daytimetcpcli.cpp ${OBJS} ${HEAD}
		${CC} ${CFLAGS} -o $@ ./src/daytimetcpcli.cpp ${OBJS} ${LIBS}

error.o: error.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/error.c ${LIBS} 
		
wrapsock.o: wrapsock.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapsock.c ${LIBS} 
		
wrapunix.o: wrapunix.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapunix.c ${LIBS} 
		
wrapstdio.o: wrapstdio.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapstdio.c ${LIBS} 
		
wraplib.o: wraplib.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wraplib.c ${LIBS} 

		
.PHONY: clean
clean:
		rm ${TARGET} ${OBJS} -rf




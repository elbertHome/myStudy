CC  = g++ 
INCLUDES = ./includes
LIBS =
CFLAGS = -I${INCLUDES} -O0 -g3 -Wall -std=c++0x
OBJS = sigchldwait.o error.o wrapsock.o wrapunix.o wrapstdio.o \
       str_echo.o str_cli.o readline.o writen.o signal.o wraplib.o
HEADS = unp.h config.h
TARGET = tcpserv01 tcpcli01

vpath %.c ./src
vpath %.c ./libs
vpath %.h ${INCLUDES}
vpath %.cpp ./src
vpath %.cpp ./libs
vpath %.hpp ${INCLUDES}


all:	${TARGET}

tcpserv01: tcpserv01.cpp ${OBJS} ${HEAD}
		${CC} ${CFLAGS} -o $@ ./src/tcpserv01.cpp ${OBJS} ${LIBS}
		
tcpcli01: tcpcli01.cpp ${OBJS} ${HEAD}
		${CC} ${CFLAGS} -o $@ ./src/tcpcli01.cpp ${OBJS} ${LIBS}
		
sigchldwait.o: sigchldwait.cpp ${HEAD}
		${CC} ${CFLAGS} -c ./src/sigchldwait.cpp ${LIBS} 

error.o: error.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/error.c ${LIBS} 
		
wrapsock.o: wrapsock.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapsock.c ${LIBS} 
		
wrapunix.o: wrapunix.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapunix.c ${LIBS} 
		
wrapstdio.o: wrapstdio.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wrapstdio.c ${LIBS}

str_cli.o: str_cli.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/str_cli.c ${LIBS} 

str_echo.o: str_echo.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/str_echo.c ${LIBS} 

readline.o: readline.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/readline.c ${LIBS} 
		
writen.o: writen.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/writen.c ${LIBS}
		
signal.o: signal.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/signal.c ${LIBS} 

wraplib.o: wraplib.c ${HEAD}
		${CC} ${CFLAGS} -c ./libs/wraplib.c ${LIBS}

clean:
		rm ${TARGET} ${OBJS} -rf




# Makefile for nmea2svg

CC = gcc
CFLAGS = `xml2-config --cflags --libs` -g

all: nmea2svg

nmea2svg: nmea.o nmea-cb.o main.o svg.o
	${CC} ${CFLAGS} -o nmea2svg nmea.o nmea-cb.o main.o svg.o

nmea: nmea.c nmea.h misc.h nmea-cb.h
	${CC} ${CFLAGS} -c nmea.c
	
nmea-cb: nmea-cb.c nmea-cb.h misc.h svg.h
	${CC} ${CFLAGS} -c nmea-cb.c

svg: svg.c svg.h
	${CC} ${CFLAGS} -c svg.c

main: main.c misc.h nmea.h nmea-cb.h
	${CC} ${CFLAGS} -c main.c

clean:
	rm -f *.o nmeasvg test.svg core

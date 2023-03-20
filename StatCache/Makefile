CC = g++

#CFLAGS for old 32-bit servers
CFLAGS = -g -O3 -msse2 -march=pentium4 -fprefetch-loop-arrays 

#CFLAGS for new 32-bit and all 64-bit servers
#CFLAGS = -g -O3 -msse3 -march=nocona -fprefetch-loop-arrays 

solver: statcache.c
	${CC} ${CFLAGS} statcache.c -o solver -lm

all: 	solver

clean:
	rm -f *.o; rm -f *~; rm -f solver ;


CC = g++

#CFLAGS for old 32-bit servers
CFLAGS = -g -O3 -msse2 -m32 -fprefetch-loop-arrays 

#-march=pentium4
#CFLAGS for new 32-bit and all 64-bit servers
#CFLAGS = -g -O3 -msse3 -march=nocona -fprefetch-loop-arrays 

solver: statcache.c
	${CC} ${CFLAGS} statcache.c -o solver -lm

my_solver: second_solver.cpp 
	${CC} ${CFLAGS} second_solver.cpp -o solver2 -lm

all: 	solver my_solver

clean:
	rm -f *.o; rm -f *~; rm -f solver ;


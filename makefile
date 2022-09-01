all: medianmode

medianmode:
	gcc -o sort medianmode.c -lrt -lpthread

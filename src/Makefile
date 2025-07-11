CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD:=.build

all: mkbuilddir lotormath

mkbuilddir:
	mkdir -p ${BUILD}

lotormath:
	${CC} -c lotormath.c -o ${BUILD}/lotormath.o
	${CC} -o ${BUILD}/test test_lotormath.c ${BUILD}/lotormath.o -lm
	${BUILD}/test

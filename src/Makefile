CC := gcc -O3 -ffast-math -Wall -pedantic -std=c11 -march=native
BUILD := .build
MODULES := mkbuilddir lotormath

all: ${MODULES} test


mkbuilddir:
	mkdir -p ${BUILD}

lotormath:
	${CC} -c lotormath.c -o ${BUILD}/lotormath.o
	${CC} -o ${BUILD}/test test_lotormath.c ${BUILD}/lotormath.o -lm

test:
	${BUILD}/test

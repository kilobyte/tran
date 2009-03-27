all:	scripts makehash

CC = gcc -Wall

SRC = $(filter-out $(wildcard gen/*~) $(wildcard gen/.*),$(wildcard gen/*))
GEN = $(SRC:gen/%=data/%)

scripts:	${GEN}

data/%:	gen/%
	$< >$@

clean:
	rm -rf ${GEN} makehash *.o

makehash: hash.o makehash.o
	${CC} -o $@ $^ -lz

hash.o makehash.o: %.o: %.c tran.h
	${CC} -c $<

.PHONY:	all scripts clean

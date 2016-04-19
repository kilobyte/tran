all:	scripts tran.1

SRC = $(filter-out $(wildcard gen/*~) $(wildcard gen/.*),$(wildcard gen/*))
GEN = $(SRC:gen/%=data/%)

scripts:	${GEN}

data/%:	gen/%
	$< >$@

tran.1:	tran.pod
	pod2man -c tran -s 1 -n tran $< >$@

clean:
	rm -rf ${GEN} tran.1

.PHONY:	all scripts clean

all:	scripts

SRC = $(filter-out $(wildcard gen/*~) $(wildcard gen/.*),$(wildcard gen/*))
GEN = $(SRC:gen/%=data/%)

scripts:	${GEN}

data/%:	gen/%
	$< >$@

clean:
	rm -rf ${GEN}

.PHONY:	all scripts clean

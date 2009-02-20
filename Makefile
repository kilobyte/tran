all:	scripts

SRC = $(filter-out $(wildcard src/*~) $(wildcard src/.*),$(wildcard src/*))
GEN = $(SRC:src/%=data/%)

scripts:	${GEN}

data/%:	src/%
	$< >$@

clean:
	rm -rf ${GEN}

.PHONY:	all scripts clean

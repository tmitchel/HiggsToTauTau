OPT=-O3
ROOT=`root-config --cflags --glibs`  -lRooFit -lRooFitCore
CFLAGS=-I${CMSSW_BASE}/src
OBIN=${CMSSW_BASE}/bin/${SCRAM_ARCH}

.PHONY: all test

all: ac-mt-2016 ac-mt-2017 ac-mt-2018 ac-et-2016 ac-et-2017 ac-et-2018 boost-mt-2017

test: test-ac-mt-2016 test-ac-mt-2017 test-ac-mt-2018 test-ac-et-2016 test-ac-et-2017 test-ac-et-2018 test-ac-tt-2016 test-ac-tt-2017 test-ac-tt-2018 test-boost-mt-2017

# Anomalous Coupling Analyzers
ac-mt-2016: plugins/AC/mt_analyzer2016.cc
	g++ $(OPT) plugins/AC/mt_analyzer2016.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2016_mt

ac-mt-2017: plugins/AC/mt_analyzer2016.cc
	g++ $(OPT) plugins/AC/mt_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2017_mt

ac-mt-2018: plugins/AC/mt_analyzer2016.cc
	g++ $(OPT) plugins/AC/mt_analyzer2018.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2018_mt

ac-et-2016: plugins/AC/et_analyzer2016.cc
	g++ $(OPT) plugins/AC/et_analyzer2016.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2016_et

ac-et-2017: plugins/AC/et_analyzer2016.cc
	g++ $(OPT) plugins/AC/et_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2017_et

ac-et-2018: plugins/AC/et_analyzer2016.cc
	g++ $(OPT) plugins/AC/et_analyzer2018.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2018_et

ac-tt-2016: plugins/AC/tt_analyzer2016.cc
	g++ $(OPT) plugins/AC/tt_analyzer2016.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2016_tt

ac-tt-2017: plugins/AC/tt_analyzer2017.cc
	g++ $(OPT) plugins/AC/tt_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2017_tt

ac-tt-2018: plugins/AC/tt_analyzer2018.cc
	g++ $(OPT) plugins/AC/tt_analyzer2018.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2018_tt

# Boosted Tau Analyzers
boost-mt-2017: plugins/Boosted/mt_analyzer2017.cc
	g++ $(OPT) plugins/Boosted/mt_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/boost_mt2017
boost-em-2017: plugins/Boosted/em_analyzer2017.cc
	g++ $(OPT) plugins/Boosted/em_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/boost_em2017

# Testing Anomalous Coupling Analyzers
test-ac-mt-2016: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2016.cc $(ROOT) $(CFLAGS) -o test

test-ac-mt-2017: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2017.cc $(ROOT) $(CFLAGS) -o test

test-ac-mt-2018: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2018.cc $(ROOT) $(CFLAGS) -o test

test-ac-et-2016: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2016.cc $(ROOT) $(CFLAGS) -o test

test-ac-et-2017: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2017.cc $(ROOT) $(CFLAGS) -o test

test-ac-et-2018: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2018.cc $(ROOT) $(CFLAGS) -o test

test-ac-tt-2016: plugins/AC/tt_analyzer2016.cc
	g++ plugins/AC/tt_analyzer2016.cc $(ROOT) $(CFLAGS) -o test

test-ac-tt-2017: plugins/AC/tt_analyzer2017.cc
	g++ plugins/AC/tt_analyzer2017.cc $(ROOT) $(CFLAGS) -o test

test-ac-tt-2018: plugins/AC/tt_analyzer2018.cc
	g++ plugins/AC/tt_analyzer2018.cc $(ROOT) $(CFLAGS) -o test

# Testing Boosted Tau Analyzers
test-boost-mt-2017: plugins/Boosted/mt_analyzer2017.cc
	g++ plugins/Boosted/mt_analyzer2017.cc $(ROOT) $(CFLAGS) -o test
test-boost-em-2017: plugins/Boosted/em_analyzer2017.cc
	g++ plugins/Boosted/em_analyzer2017.cc $(ROOT) $(CFLAGS) -o test

# Clean binaries
clean:
	rm $(OBIN)/*

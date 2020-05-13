OPT=-O3
ROOT=`root-config --cflags --glibs`  -lRooFit -lRooFitCore
CFLAGS=-I${CMSSW_BASE}/src
OBIN=${CMSSW_BASE}/bin

all: ac-mt-2016 ac-mt-2017 ac-mt-2018 ac-et-2016 ac-et-2017 ac-et-2018

test: test-ac-mt-2016 test-ac-mt-2017 test-ac-mt-2018 test-ac-et-2016 test-ac-et-2017 test-ac-et-2018 

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

test-ac-mt-2016: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2016.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2016_mt

test-ac-mt-2017: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2017_mt

test-ac-mt-2018: plugins/AC/mt_analyzer2016.cc
	g++ plugins/AC/mt_analyzer2018.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2018_mt

test-ac-et-2016: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2016.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2016_et

test-ac-et-2017: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2017.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2017_et

test-ac-et-2018: plugins/AC/et_analyzer2016.cc
	g++ plugins/AC/et_analyzer2018.cc $(ROOT) $(CFLAGS) -o $(OBIN)/analyze2018_et

clean:
	rm $(OBIN)/*

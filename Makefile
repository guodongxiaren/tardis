PROTODIR=proto/
PROTOFILE=$(wildcard $(PROTODIR)/*.proto)
PROTOSRC=$(patsubst %.proto,%.pb.cc,$(PROTOFILE))
PROTOOBJS=$(patsubst %.cc,%.o, $(PROTOSRC))

DEMO=demo

ALL: ENV $(DEMO) test_common test_dict

$(DEMO): $(PROTOOBJS) demo/*.cpp
	g++ -std=c++11 $^ -I include -I proto -lglog -lprotobuf -o output/bin/$@


ENV:
	mkdir -p output/bin
	mkdir -p output/lib
	mkdir -p output/include
	mkdir -p output/test
	mkdir -p output/test/data

$(PROTOSRC): $(PROTOFILE)
	cd $(PROTODIR);protoc --cpp_out=. $(notdir $(PROTOFILE))
	@echo $(shell pwd)

%.o: %.cpp 
	g++ -c $^ -o $@ -I $(PROTODIR)

test_common: test/case/test_common.cpp
	g++ $^ -o output/test/$@ -std=c++11 -I include -I proto -lglog -lprotobuf -lgtest -pthread

test_dict: test/case/test_dict.cpp $(PROTOOBJS)
	g++ $^ -o output/test/$@ -std=c++11 -I include -I proto -lglog -lprotobuf -lgtest -pthread
	cp test/data/* output/test/data/

clean:
	cd $(PROTODIR); rm -f *.h; rm -f *.cc; rm -f *.o
	rm -rf output

PROTODIR=proto/
PROTOFILE=$(wildcard $(PROTODIR)/*.proto)
PROTOSRC=$(patsubst %.proto,%.pb.cc,$(PROTOFILE))
PROTOOBJS=$(patsubst %.cc,%.o, $(PROTOSRC))

DEMO=demo

ALL: ENV $(DEMO)

$(DEMO): $(PROTOOBJS)
	g++ -std=c++11 demo/*.cpp proto/*.o -I include -I proto -lglog -lprotobuf -o output/bin/$@


ENV:
	mkdir -p output/bin
	mkdir -p output/lib
	mkdir -p output/include

$(PROTOSRC): $(PROTOFILE)
	cd $(PROTODIR);protoc --cpp_out=. $(notdir $(PROTOFILE))
	@echo $(shell pwd)

%.o: %.cpp 
	g++ -c $^ -o $@ -I $(PROTODIR)

clean:
	cd $(PROTODIR); rm -f *.h; rm -f *.cc; rm -f *.o
	rm -rf output

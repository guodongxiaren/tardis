PROTODIR=proto/
PROTOFILE=$(wildcard $(PROTODIR)/*.proto)
PROTOSRC=$(patsubst %.proto,%.pb.cc,$(PROTOFILE))
PROTOOBJS=$(patsubst %.cc,%.o, $(PROTOSRC))

SRC_DIR=commdict/
SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(patsubst %.cpp,%.o, $(SRCS))

DEMO=demo

ALL: ENV $(DEMO)

$(DEMO): $(PROTOOBJS) $(OBJS)
	g++ demo/*.cpp proto/*.o -I commdict -I proto -lglog -lprotobuf -o output/bin/$@


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
	cd $(SRC_DIR); rm -f *.o
	rm -rf output

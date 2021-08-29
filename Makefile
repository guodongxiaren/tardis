PROTO_DIR=proto
OUTPUT=output
BIN_DIR=$(OUTPUT)/bin
TEST_DIR=test
PROTOFILE=$(wildcard $(PROTO_DIR)/*.proto)
PROTOSRC=$(patsubst %.proto,%.pb.cc,$(PROTOFILE))
PROTOOBJS=$(patsubst %.cc,%.o, $(PROTOSRC))
TEST_SRC=$(wildcard $(TEST_DIR)/case/*.cpp)
TEST_BIN=$(patsubst %.cpp,%,$(TEST_SRC))
CC=g++
CXX_FLAG=-std=c++11 -O2 -Wno-unused-result

DEMO=$(BIN_DIR)/demo

#ALL: ENV $(DEMO) test_common test_dict
ALL: ENV $(DEMO) $(TEST_BIN)

$(DEMO): $(PROTOOBJS) demo/demo.cpp
	$(CC) $(CXX_FLAG) $^ -I include -I proto -lglog -lprotobuf -o $@


ENV:
	mkdir -p output/bin
	mkdir -p output/lib
	mkdir -p output/include
	mkdir -p output/test
	mkdir -p output/test/data

$(PROTOSRC): $(PROTOFILE)
	cd $(PROTO_DIR);protoc --cpp_out=. $(notdir $(PROTOFILE))
	@echo $(shell pwd)

%.o: %.cpp 
	$(CC) $(CXX_FLAG) -c $^ -o $@ -I $(PROTO_DIR)

test/case/test_%: test/case/test_%.cpp $(PROTOOBJS)
	$(CC) $(CXX_FLAG) $^ -o $@ -I include -I proto -lglog -lprotobuf -lgtest -pthread

clean:
	cd $(PROTO_DIR); rm -f *.h; rm -f *.cc; rm -f *.o
	rm -rf output

PROTO_DIR=proto
OUTPUT=output
BIN_DIR=$(OUTPUT)/bin
TEST_DIR=test
PROTO_FILE=$(wildcard $(PROTO_DIR)/*.proto)
PROTO_SRC=$(patsubst %.proto,%.pb.cc,$(PROTO_FILE))
PROTO_OBJ=$(patsubst %.cc,%.o, $(PROTO_SRC))
TEST_SRC=$(wildcard $(TEST_DIR)/*.cpp)
TEST_BIN=$(patsubst %.cpp,%,$(TEST_SRC))
CC=g++
CXX_FLAG=-std=c++11 -O2 -Wno-unused-result

DEMO=$(BIN_DIR)/demo

#ALL: ENV $(DEMO) test_common test_dict
ALL: ENV $(DEMO) $(TEST_BIN)

$(DEMO): $(PROTO_OBJ) demo/demo.cpp
	$(CC) $(CXX_FLAG) $^ -I include -I proto -lglog -lprotobuf -o $@


ENV:
	mkdir -p output/bin
	mkdir -p output/lib
	mkdir -p output/include
	mkdir -p output/test
	mkdir -p output/test/data

$(PROTO_SRC): $(PROTO_FILE)
	cd $(PROTO_DIR);protoc --cpp_out=. $(notdir $(PROTO_FILE))
	@echo $(shell pwd)

%.o: %.cpp 
	$(CC) $(CXX_FLAG) -c $^ -o $@ -I $(PROTO_DIR)

test/test_%: test/test_%.cpp $(PROTO_OBJ)
	$(CC) $(CXX_FLAG) $^ -o $@ -I include -I proto -lglog -lprotobuf -lgtest -pthread

clean:
	cd $(PROTO_DIR); rm -f *.h; rm -f *.cc; rm -f *.o
	rm -rf output

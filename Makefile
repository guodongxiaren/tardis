PROTO_DIR=proto
OUTPUT=output
BIN_DIR=$(OUTPUT)/bin
TEST_DIR=test
PROTO_FILE=$(wildcard $(PROTO_DIR)/*.proto)
PROTO_SRC=$(patsubst %.proto,%.pb.cc,$(PROTO_FILE))
PROTO_OBJ=$(patsubst %.cc,%.o, $(PROTO_SRC))
TEST_SRC=$(wildcard $(TEST_DIR)/*.cpp)
TEST_BIN=$(patsubst %.cpp,%,$(TEST_SRC))
TARDIS_INC=include
TARDIS_LIB=output/lib/libtardis.a
CC=g++
CXX_FLAG=-std=c++11 -O2 -Wno-unused-result

DEMO=$(BIN_DIR)/demo

ALL: ENV tardis $(DEMO) $(TEST_BIN)

tardis: $(TARDIS_LIB)

$(TARDIS_LIB):
	cd $(TARDIS_INC) && protoc --cpp_out=. tardis/tardis.proto
	@echo $(shell pwd)
	$(CC) $(CXX_FLAG) -c $(TARDIS_INC)/tardis/tardis.pb.cc -I include -o output/lib/tardis.pb.o
	ar -rv $@ output/lib/tardis.pb.o

$(DEMO): demo/demo.cpp $(TARDIS_LIB) $(PROTO_OBJ) 
	$(CC) $(CXX_FLAG) $^ -I include -I proto -lglog -lprotobuf -o $@


ENV:
	mkdir -p output/bin
	mkdir -p output/lib
	mkdir -p output/include
	mkdir -p output/test
	mkdir -p output/test/data

$(PROTO_SRC): $(PROTO_FILE)
	protoc -I=include/ -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $(PROTO_DIR)/*.proto
	@echo $(shell pwd)

%.pb.o: %.pb.cc 
	$(CC) $(CXX_FLAG) -c $^ -o $@ -I $(PROTO_DIR) -I $(TARDIS_INC)

%.o: %.cpp 
	$(CC) $(CXX_FLAG) -c $^ -o $@ -I $(PROTO_DIR) -I $(TARDIS_INC)

test/test_%: test/test_%.cpp $(PROTO_OBJ) $(TARDIS_LIB)
	$(CC) $(CXX_FLAG) $^ -o $@ -I include -I proto -lglog -lprotobuf -lgtest -pthread

clean:
	rm -f *.out; rm -f *.o; rm -f *.a
	cd $(PROTO_DIR); rm -f *.h; rm -f *.cc; rm -f *.o
	rm -rf output

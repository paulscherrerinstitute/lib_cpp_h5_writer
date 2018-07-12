SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
MKDIR = mkdir -p

CPP = g++
CPPFLAGS = -Wall -Wfatal-errors -fPIC -pthread -std=c++11 -I./include -I${CONDA_PREFIX}/include
LDLIBS = -L${CONDA_PREFIX}/lib -L/usr/lib64 -lzmq -lhdf5 -lhdf5_hl -lhdf5_cpp -lhdf5_hl_cpp -lboost_system -lboost_regex -lboost_thread -lpthread
LDLIBS_TEST = -lgtest_main -lgtest
LDFLAGS = -shared

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LDFLAGS += -Wl,-soname,libcpp_h5_writer.so
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -Wl,-install_name,libcpp_h5_writer.so
endif

HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

libcpp_h5_writer: build_dirs $(OBJS)
	$(CPP) $(LDFLAGS) -o $(BIN_DIR)/libcpp_h5_writer.so $(OBJS) $(LDLIBS)

debug: CPPFLAGS += -DDEBUG_OUTPUT
debug: lib

perf: CPPFLAGS += -DPERF_OUTPUT
perf: lib

deploy: libcpp_h5_writer
	cp bin/* ${CONDA_PREFIX}/lib
	mkdir -p ${CONDA_PREFIX}/include/cpp_h5_writer
	cp src/*.hpp ${CONDA_PREFIX}/include/cpp_h5_writer
	cp include/date.h ${CONDA_PREFIX}/include/cpp_h5_writer

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) $(LDLIBS) -c -o $@ $<

build_dirs:
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

test: build_dirs $(OBJS)
	$(CPP) $(CPPFLAGS) test/test_main.cpp $(OBJS) $(LDLIBS) $(LDLIBS_TEST) -o $(BIN_DIR)/execute_tests
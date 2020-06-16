SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
MKDIR = mkdir -p

# include_directories(${HDF5_INCLUDE_DIRS})

#CC = g++
CFLAGS = -DLIBCXX_CXX_ABI=libstdc++ -Wall -Wfatal-errors -fPIC -pthread -std=c++11 -I./include -I${CONDA_PREFIX}/include
LDFLAGS = -L${CONDA_PREFIX}/lib -L${CONDA_PREFIX}/lib64  -lzmq -lhdf5 -lhdf5_cpp -lhdf5_hl -lhdf5_hl_cpp -lboost_system -lboost_regex -lboost_thread -lpthread -lboost_chrono

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	SOFLAGS += -shared -Wl,-soname,libcpp_h5_writer.so
endif
ifeq ($(UNAME), Darwin)
	SOFLAGS += -shared -Wl,-install_name,libcpp_h5_writer.so
endif

HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

libcpp_h5_writer: build_dirs $(OBJS)
	$(CC) $(SOFLAGS) -o $(BIN_DIR)/libcpp_h5_writer.so $(OBJS) $(LDFLAGS)

debug: CFLAGS += -DDEBUG_OUTPUT
debug: libcpp_h5_writer

perf: CFLAGS += -DPERF_OUTPUT
perf: libcpp_h5_writer

deploy: libcpp_h5_writer
	cp bin/* ${CONDA_PREFIX}/lib
	mkdir -p ${CONDA_PREFIX}/include/cpp_h5_writer
	cp src/*.hpp ${CONDA_PREFIX}/include/cpp_h5_writer
	cp include/date.h ${CONDA_PREFIX}/include/cpp_h5_writer

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

build_dirs:
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

test: build_dirs $(OBJS)
	$(CC) $(CFLAGS) test/test_main.cpp $(OBJS) $(LDFLAGS) -lgtest_main -lgtest -o $(BIN_DIR)/execute_tests

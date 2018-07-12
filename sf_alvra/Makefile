SRC_DIR = .
OBJ_DIR = ./obj
BIN_DIR = ./bin
MKDIR = mkdir -p

CC = g++
CFLAGS = -Wall -Wfatal-errors -std=c++11 -I${CONDA_PREFIX}/include -I${CONDA_PREFIX}/include/cpp_h5_writer
LDFLAGS = -L${CONDA_PREFIX}/lib -L/usr/lib64 -lcpp_h5_writer -lzmq -lhdf5 -lhdf5_hl -lhdf5_cpp -lhdf5_hl_cpp -lboost_system -lboost_regex -lboost_thread -lpthread

HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

alvra: lib all

lib:
	$(MAKE) -C ../lib deploy

all: build_dirs alvra_h5_writer

debug: CFLAGS += -DDEBUG_OUTPUT -g
debug: alvra

deploy: alvra
	cp bin/* ${CONDA_PREFIX}/bin

alvra_h5_writer: $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/alvra_h5_writer $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

build_dirs:
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
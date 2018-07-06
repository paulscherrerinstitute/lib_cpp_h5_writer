SRC_DIR = .
OBJ_DIR = ./obj
BIN_DIR = ./bin
MKDIR = mkdir -p

CPP = g++
CPPFLAGS = -Wall -Wfatal-errors -fPIC -pthread -std=c++11 -I../../src -I${CONDA_PREFIX}/include
LDLIBS = -L../../bin -L${CONDA_PREFIX}/lib -L/usr/lib64 -lcpp_h5_writer -lzmq -lhdf5 -lhdf5_hl -lhdf5_cpp -lhdf5_hl_cpp -lboost_system -lboost_regex -lboost_thread -lpthread
LDLIBS_TEST = -lgtest_main -lgtest

HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

bernina: lib all

lib:
	$(MAKE) -C ../../ lib

all: build_dirs bernina_h5_writer

debug: CPPFLAGS += -DDEBUG_OUTPUT -g
debug: bernina_h5_writer

bernina_h5_writer: $(OBJS)
	$(CPP) $(LDFLAGS) -o $(BIN_DIR)/bernina_h5_writer $(OBJS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) $(LDLIBS) -c -o $@ $<

build_dirs:
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	$(MAKE) -C ../../ clean
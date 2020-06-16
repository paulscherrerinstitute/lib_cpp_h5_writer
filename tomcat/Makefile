SRC_DIR = .
OBJ_DIR = ./obj
BIN_DIR = ./bin
MKDIR = mkdir -p

#CC = g++
CFLAGS = -DLIBCXX_CXX_ABI=libstdc++ -Wall -Wfatal-errors -std=c++11 -I${CONDA_PREFIX}/include -I${CONDA_PREFIX}/include/cpp_h5_writer
LDFLAGS = -L${CONDA_PREFIX}/lib -L${CONDA_PREFIX}/lib64/ -lcpp_h5_writer -lzmq -lhdf5 -lhdf5_cpp -lhdf5_hl_cpp -lhdf5_hl -lboost_system -lboost_regex -lboost_thread -lpthread -lboost_chrono -lstdc++

HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

tomcat_h5_writer: export LD_LIBRARY_PATH=${CONDA_PREFIX}/lib
tomcat_h5_writer: build_dirs $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/tomcat_h5_writer $(OBJS) $(LDFLAGS)

#lib:
#	$(MAKE) -C /home/hax_l/debug/lib_cpp_h5_writer/lib debug deploy

debug: CFLAGS += -DDEBUG_OUTPUT -g
debug: tomcat_h5_writer

deploy: tomcat_h5_writer
	cp bin/* ${CONDA_PREFIX}/bin

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

build_dirs:
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

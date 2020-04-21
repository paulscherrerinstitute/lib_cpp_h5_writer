make

cp bin/* ${PREFIX}/lib

mkdir -p ${PREFIX}/include/cpp_h5_writer
cp src/*.hpp ${PREFIX}/include/cpp_h5_writer
cp include/date.h ${CONDA_PREFIX}/include/cpp_h5_writer

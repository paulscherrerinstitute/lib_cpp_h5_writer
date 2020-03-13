import h5py
file = h5py.File("/Users/lhdamiani/Software/Tomcat/lib_cpp_h5_writer/tomcat/outputfile.h5", 'r')
det_ty = file['detector']
for i in det_ty:
    print(i)

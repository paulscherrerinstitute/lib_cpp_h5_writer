FROM lhdamiani/tomcat_docker:latest
LABEL maintainer="Leonardo Hax <leonardo.hax@psi.ch>"
WORKDIR /
RUN git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git \
  && cd /lib_cpp_h5_writer \
  && git fetch \
  && git checkout tomcat \
  && cd /lib_cpp_h5_writer/tomcat \
  && make \
  && ln -s /lib_cpp_h5_writer/lib/bin/libcpp_h5_writer.so /lib64/ \
  && tar -zxf /lib_cpp_h5_writer/tomcat/pco.edge.tar.gz

RUN ["chmod", "+x", "/lib_cpp_h5_writer/tomcat/test_simulation_h5_writer.sh"]

ENTRYPOINT ["/bin/bash", "-c", "./lib_cpp_h5_writer/tomcat/docker-entrypoint.sh"]

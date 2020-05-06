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

COPY test_simulation_h5_writer.sh /tmp/test_simulation_h5_writer.sh
RUN ["chmod", "+x", "/tmp/test_simulation_h5_writer.sh"]

COPY docker-entrypoint.sh /tmp/docker-entrypoint.sh
ENTRYPOINT ["/bin/bash", "-c", "./tmp/docker-entrypoint.sh"]

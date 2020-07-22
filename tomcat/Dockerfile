FROM lhdamiani/tomcat_docker:latest
LABEL maintainer="Leonardo Hax <leonardo.hax@psi.ch>"
WORKDIR /
RUN rm -rf /lib_cpp_h5_writer/ \
  && git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git \
  && cd /lib_cpp_h5_writer \
  && git fetch \
  && git checkout tomcat \
  && cd /lib_cpp_h5_writer/lib \
  && make debug deploy\
  && cd /lib_cpp_h5_writer/tomcat \
  && make debug \

RUN ["/bin/bash", "-c", "chmod +x /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/test_simulation_h5_writer.sh"]

ENTRYPOINT ["/bin/bash"]

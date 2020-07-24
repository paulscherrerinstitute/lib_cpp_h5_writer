FROM lhdamiani/tomcat_writer:latest
LABEL maintainer="Leonardo Hax <leonardo.hax@psi.ch>"
WORKDIR /

RUN yes | yum install git

RUN if [ -d "/lib_cpp_h5_writer" ]; then rm -Rf /lib_cpp_h5_writer; fi \
  && git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git \
  && cd /lib_cpp_h5_writer \
  && git fetch \
  && git checkout tomcat \

CMD chmod +x /lib_cpp_h5_writer/tomcat/dockerfile/test_simulation_h5_writer.sh

CMD /lib_cpp_h5_writer/tomcat/dockerfile/test_simulation_h5_writer.sh

ENTRYPOINT ["/bin/bash"]

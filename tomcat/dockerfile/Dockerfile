FROM lhdamiani/tomcat_writer:latest
LABEL maintainer="Leonardo Hax <leonardo.hax@psi.ch>"
WORKDIR /

RUN yes | yum install git

RUN rm -Rf /lib_cpp_h5_writer \
  && git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git \
  && cd /lib_cpp_h5_writer/ \
  && git fetch \
  && git checkout tomcat \
  && chmod +x /lib_cpp_h5_writer/tomcat/dockerfile/test_simulation_h5_writer.sh \
  && cd /lib_cpp_h5_writer/tomcat/dockerfile/
  
RUN /lib_cpp_h5_writer/tomcat/dockerfile/test_simulation_h5_writer.sh 

ENTRYPOINT ["/bin/bash"]

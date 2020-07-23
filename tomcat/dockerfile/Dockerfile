FROM lhdamiani/tomcat_docker:latest
LABEL maintainer="Leonardo Hax <leonardo.hax@psi.ch>"
WORKDIR /

COPY ./test_simulation_h5_writer.sh /test_simulation_h5_writer.sh
COPY ./tomcat_env.yml /tomcat_env.yml
COPY ./mflow_env.yml /mflow_env.yml

RUN ["/bin/bash", "-c", "chmod +x /test_simulation_h5_writer.sh"]

# ENTRYPOINT ["/bin/bash"]
ENTRYPOINT ["/bin/bash", "-c", "/test_simulation_h5_writer.sh"]
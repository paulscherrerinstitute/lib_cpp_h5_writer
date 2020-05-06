#!/bin/bash
  
# turn on bash's job control
set -m

# # Start the newly build writer from the latest commit https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git 
/tmp/lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer tcp://localhost:9999 output_new.h5 2 -1 1 &
  
# Replays the pco.edge stream based on https://github.com/paulscherrerinstitute/mflow/blob/master/mflow/utils/replay.py
conda activate mflow
m_replay /tmp/lib_cpp_h5_writer/tomcat/tmp/mdump -a tcp://127.0.0.1:9999
conda deactivate mflow

# sleep 2
sleep 2

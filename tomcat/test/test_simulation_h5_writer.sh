#!/bin/bash
  
# turn on bash's job control
set -m
set -e

receive_stream() {
    echo "STREAMING/RECEIVING: $1..."
    echo "Starting the writer on the background..."
    # Start the writer from the latest commit https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git 
    source activate /root/miniconda2/envs/tomcat_env/
    nohup /lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer tcp://localhost:9999 output.h5 50 -1 1 9555 $1 20000 tcp://*:8088 &
    # nohup /home/hax_l/software/lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer tcp://localhost:9999 output.h5 50 -1 1 9555 $1 20000 tcp://*:8088 &
    conda deactivate

    # Replays the pco.edge stream based on https://github.com/paulscherrerinstitute/mflow/blob/master/mflow/utils/replay.py
    source activate mflow
    # unpack the data to replay
    if [ -d "/lib_cpp_h5_writer/tomcat/tests/dump/" ] 
    # if [ -d "/home/hax_l/lib_cpp_h5_writer/tomcat/test/dump/" ] 
    then
        # delete previously existing folder
        yes | rm -rf /lib_cpp_h5_writer/tomcat/tests/dump/
        # yes | rm -rf /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/dump/
    fi
    # unpack the compressed data to replay
    tar xvzf /lib_cpp_h5_writer/tomcat/tests/dump.tar.gz 
    # tar xvzf /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/dump.tar.gz 
    nohup m_replay /lib_cpp_h5_writer/tomcat/tests/dump/ -a tcp://127.0.0.1:9999 & 
    # nohup m_replay /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/dump/ -a tcp://127.0.0.1:9999 & 
    conda deactivate

    # sleep 2
    echo 'sleep 1'
    sleep 2
    echo 'Stopping the writer...'
    curl -X GET http://0.0.0.0:9555/stop
    echo 'sleep 3'
    sleep 3


}

FILE=/lib_cpp_h5_writer/tomcat/test/output.h5
# FILE=/home/hax_l/software/lib_cpp_h5_writer/tomcat/test/output.h5
if [ -f "$FILE" ]; then
    echo "Deleting previously existant output file..."
    yes | rm $FILE
fi
# send stream data
receive_stream "data"
# send stream data_white
receive_stream "data_white"
# send stream data_black
receive_stream "data_black"

# Baisc verification - if both files have the same size
I=`stat -c%s /lib_cpp_h5_writer/tomcat/test/output.h5 `
# I=`stat -c%s /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/output.h5 `
J=`stat -c%s /lib_cpp_h5_writer/tomcat/test/output_ref.h5 `
# J=`stat -c%s /home/hax_l/software/lib_cpp_h5_writer/tomcat/test/output_ref.h5 `
if [[ ! "$I" -eq "$J" ]] ; 
then
    echo "FILES ARE NOT THE SAME SIZE $I $J"  
    exit -1
fi
# all good
exit 0
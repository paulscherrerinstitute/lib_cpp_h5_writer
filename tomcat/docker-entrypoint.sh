#!/bin/bash
echo -e "\n##########################################################"
echo -e "#### Welcome to dockerized lib_cpp_H5_writer : TOMCAT ####"
echo -e "##########################################################\n"
echo -e "Source code located in /lib_cpp_h5_writer/"
echo -e "\nTo start the writer run:"
echo -e "./tomcat_h5_writer tcp://<PCO_ADDRESS>:<PORT> <OUTPUT_FILENAME>.h5 <N_FRAMES> <USER_ID> <N_MODULES>\n"

cd /lib_cpp_h5_writer/tomcat/bin
echo -e "Current path: $PWD"
/bin/bash 

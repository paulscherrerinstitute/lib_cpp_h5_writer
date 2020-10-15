######################################
Getting started with TOMCAT pco writer
######################################

PCO client controller (pco_rclient)
-----------------------------------
To verify if the pco_rclient is installed and working on your python virtual environment:

.. code-block:: python
    
    $ python -c "from pco_rclient import PcoWriter;

TOMCAT PCO writer client
------------------------
The pco writer client was developed with the intention to allow a fluid manipulation of the camera commands in a trustful and robust way. It communicates via a REST API with a python flask server that runs on xbl-daq-32 and controls the instantiation of the writer library (lib_cpp_h5_writer) as an external process. 
The list of parameters are displayed in the table below:

+---------------------------+-----------------------------------------------------+
| Writer parameter          | Description                                         |
+===========================+=====================================================+
| output_file               | Output file name                                    |
+---------------------------+-----------------------------------------------------+
| dataset_name              | Dataset name (data, data_black, data_white)         |
+---------------------------+-----------------------------------------------------+
| n_frames                  | Total number of frames expected                     |
+---------------------------+-----------------------------------------------------+
| connection_address        | Camera server address (incoming ZMQ stream)         |
+---------------------------+-----------------------------------------------------+
| flask_api_address         | Flask server's address (http://xbl-daq-32:9901)     |
+---------------------------+-----------------------------------------------------+
| writer_api_address        | Writer REST api address (http://xbl-daq-32:9555)    |
+---------------------------+-----------------------------------------------------+
| user_id                   | User id                                             |
+---------------------------+-----------------------------------------------------+
| max_frames_per_file       | Defines the max frames on each h5 file              |
+---------------------------+-----------------------------------------------------+
| debug                     | Runs the client in debug mode                       |
+---------------------------+-----------------------------------------------------+

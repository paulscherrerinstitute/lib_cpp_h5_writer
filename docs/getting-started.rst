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
 
Basic example:

.. code-block:: python

    pco_controller = PcoWriter(connection_address=<CAMERA_ADDRESS>, user_id=<USER_ID>)
    pco_controller.get_status()

Writer parameters:

| Name  |  Description  |
|---|---|
| output_file  | Output file name.  |
| dataset_name  | Dataset name (data, data_black, data_white)  |
| n_frames  | Total number of frames expected.  |
| connection_address  | Address of the camera server, where the incoming ZMQ stream is generated (tcp://129.129.99.104:8080)   |
| flask_api_address  | Address of the flask server (http://xbl-daq-32:9901)  |
| writer_api_address  | Address of the writer (http://xbl-daq-32:9555)  |
| user_id  | User id  |
| max_frames_per_file  | Defines the max frames on each file (h5 output with multiple chunked files)  |
| debug  | Runs the client with the pre-defined local debug configuration.  |

TOMCAT PCO Cameras servers / IOC's name
---------------------------------------

There are currently two servers installed with PCO cameras: PCO-3 and PCO-4.

    * ``tcp://129.129.99.104:8080`` : the 1G copper link on x02da-pco-4 (last updated: 2020-09-31)
    * ``tcp://pc9808:9999`` : Debug pco camera.

IOC's name:
    * X02DA-CCDCAM2
    * X02DA-CCDCAM3

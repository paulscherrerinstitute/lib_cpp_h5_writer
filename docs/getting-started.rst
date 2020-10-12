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
   :linenos:

    pco_controller = PcoWriter(connection_address="tcp://129.129.99.104:8080", 
                            user_id=user_id)
    pco_controller.get_status()


TOMCAT PCO writer Rest API
------------------------------------

The direct calls to the REST Api will be shown with cURL.

.. code-block:: python

    # Get writer status
    curl -X GET http://xbl-daq-32:9555/status

    # Get server status
    curl -X GET http://xbl-daq-32:9901/status


TOMCAT PCO Cameras servers / IOC's name
---------------------------------------

There are currently two servers installed with PCO cameras: PCO-3 and PCO-4.

    * ```tcp://129.129.99.104:8080``` : the 1G copper link on x02da-pco-4 (last updated: 2020-09-31)
    * //TODO PUT the complete/official tcp addresses here
    * DEBUG: ```tcp://pc9808:9999``` : Debug pco camera.

IOC's name:
    * X02DA-CCDCAM2
    * X02DA-CCDCAM3

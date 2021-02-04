###########
Development
###########

Cameras/server addresses PCO1
-----------------------------

+---------------------------+-----------------------------------------------------+
| Address:Port              | Description                                         |
+===========================+=====================================================+
| tcp://129.129.99.47:8080  | PCO camera (1G copper link X02DA-PCO-1.psi.ch)      |
+---------------------------+-----------------------------------------------------+
| tcp://10.10.1.26:8080     | PCO camera (10G fiber-optic link X02DA-PCO-1.psi.ch)|
+---------------------------+-----------------------------------------------------+
| X02DA-CCDCAM1             | PCO camera 1 ioc                                    |
+---------------------------+-----------------------------------------------------+
| pco_writer-pco1.          | Service name for pco 1                              |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-34:9901    | Flask python server for pco 1                       |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9555    | Writer API address (only valid during acquisitions)*|
+---------------------------+-----------------------------------------------------+

Cameras/server addresses PCO2
-----------------------------

+---------------------------+-----------------------------------------------------+
| Address:Port              | Description                                         |
+===========================+=====================================================+
| tcp://129.129.99.107:8080 | PCO camera (1G copper link X02DA-PCO-2.psi.ch)      |
+---------------------------+-----------------------------------------------------+
| tcp://10.10.1.202:8080    | PCO camera (10G fiber-optic link X02DA-PCO-2.psi.ch)|
+---------------------------+-----------------------------------------------------+
| X02DA-CCDCAM2             | PCO camera 2 ioc                                    |
+---------------------------+-----------------------------------------------------+
| pco_writer-pco2.          | Service name for pco 2                              |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-34:9902    | Flask python server for pco 2                       |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9556    | Writer API address (only valid during acquisitions)*|
+---------------------------+-----------------------------------------------------+

.. note::
   * *Writer API address is a parameter used when configuring and, therefore, can be changed.
   
.. note::
   * *DEBUG PCO Camera: tcp://pc9808:9999

Contribute and create a merge request
-------------------------------------
You can create a new branch to implement new features for the tomcat pco writer by doing

.. code-block:: bash

    $ git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git
    $ git fetch 
    $ git checkout tomcat
    $ git pull
    $ git checkout -b <new_branch>

For the PCO writer's client, ``pco_rclient``: 

.. code-block:: bash

    $ git clone https://github.com/paulscherrerinstitute/pco_rclient.git
    $ git fetch 
    $ git pull
    $ git checkout -b <new_branch>

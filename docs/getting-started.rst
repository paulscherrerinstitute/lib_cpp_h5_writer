######################################
Getting started with TOMCAT pco writer
######################################

CaQTdm panels
-------------

It is assumed that the user is familiar with the pco edge camera's acquisition panels (informations can be found at the website `caqtdm`_ and also at `psi caqtdm page`_).

.. _caqtdm : https://caqtdm.github.io/
.. _psi caqtdm page : http://epics.web.psi.ch/software/caqtdm/


PCO client controller (pco_rclient)
-----------------------------------

Verify if the pco_rclient is installed and working:

:: code-block:: bash
    $ pco_rclient -h

Output:

:: code-block:: bash

    usage: pco_rclient [-h] {start,stop,kill,status} ...

    Rest api pco writer

    optional arguments:
    -h, --help            show this help message and exit

    command:
    valid commands

    {start,stop,kill,status}
                            commands
        start               start the writer
        stop                stop the writer
        kill                kill the writer
        status              Retrieve the status of the writer


TOMCAT PCO writer configuration file
------------------------------------
From there, create a tomcat writer pco configuration file (*.pco) according to your needs. We recommend starting with the following template and adjust it to your needs:

:: code-block:: bash

    ######################################
    # PCO.EDGE WRITER CONFIGURATION FILE #
    #####################################

    # ZMQ incoming address (from the camera)
    connection_address = tcp://129.129.95.47:8080

    # Output file name 
    output_file = output.h5

    # Total number of frames for this acquisition (0 for undefined - use the rest api to stop)
    n_frames = 0

    # User id
    user_id = -1

    # Number of n_modules
    n_modules = 1

    # Rest api port (that will connect with the pco_rclient )
    rest_api_port = 8082

    # Dataset file name
    dataset_name = data

    # Max frames per file
    max_frames_per_file = 20000

    # TCP ZMQ output statistics address
    tcp://*:8081

.. note::
    If the output file exists, data will be appended to it:
        * Make sure to change the **dataset_name** variable. If a dataset with the same name already exists on the output file, the writer will exit. 
        * Append to files at your own risk, if by any reason the second acquisition fails, the output file will be corrupted and the previously existing data will not be accessible any more.
    If the **n_frames** is 0, the writer will not stop until the client command is issued.
    If you don't know what the parameters means, we recommend to not change it.


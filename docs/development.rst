###########
Development
###########

Useful info
-----------

+---------------------------+-----------------------------------------------------+
| Address:Port              | Description                                         |
+===========================+=====================================================+
| tcp://129.129.99.104:8080 | PCO camera (1G copper link on x02da-pco-4)          |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9901    | Flask python server                                 |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9955    | Writer API address (only valid during acquisitions) |
+---------------------------+-----------------------------------------------------+
| tcp://pc9808:9999         | DEBUG PCO Camera                                    |
+---------------------------+-----------------------------------------------------+

Contribute and create a merge request
-------------------------------------
You can create a new branch to implemenet new features for the tomcat pco writer by doing

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
###########
Development
###########

Cameras/server addresses
------------------------

+---------------------------+-----------------------------------------------------+
| Address:Port              | Description                                         |
+===========================+=====================================================+
| tcp://129.129.99.47:8080  | PCO camera (10G copper link X02DA-PCO-1.psi.ch)     |
+---------------------------+-----------------------------------------------------+
| tcp://10.10.1.26:8080     | PCO camera (1G copper link X02DA-PCO-1.psi.ch)      |
+---------------------------+-----------------------------------------------------+
| tcp://10.10.1.202:8080    | PCO camera (10G copper link X02DA-PCO-2.psi.ch)     |
+---------------------------+-----------------------------------------------------+
| tcp://129.129.99.107:8080 | PCO camera (1G copper link X02DA-PCO-2.psi.ch)      |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9901    | Flask python server                                 |
+---------------------------+-----------------------------------------------------+
| http://xbl-daq-32:9555    | Writer API address (only valid during acquisitions)*|
+---------------------------+-----------------------------------------------------+
| tcp://pc9808:9999         | DEBUG PCO Camera                                    |
+---------------------------+-----------------------------------------------------+

.. note::
   * *Writer API address is a parameter used when configuring and, therefore, can be changed.

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

############
Installation
############


Tomcat pco writer
-----------------
Tomcat pco writer is deployed and installed on xbl-daq-32 using `ansible`_. The `ansible configuration`_ shows the remote connection to the server and allows to easily deploy new versions of the writer library.

    * `pco setup script`_ is used to install the TOMCAT pco writer. It will create the home folder for the specified user on the specified machine, create the conda environment based on `conda exported file`_, clone the repository (for the branch named tomcat) and compile the core library and the tomcat's format and configuration writer.

    * `pco start script`_ activates the environment and starts the service that starts a `python flask server`_. The flask server is listening to commands issued using the `pco_rclient`_.

Tomcat pco writer client
------------------------
Tomcat's pco writer controller is hosted on `PSI conda channel`_ and is installed using:

:: code-block:: bash
    $ conda install -c paulscherrerinstitute pco_rclient

.. note::
   pco_rclient needs to have access to xbl-daq-32 (where the flask server and the writer run).

.. _ansible : https://www.ansible.com/
.. _ansible configuration : https://git.psi.ch/HPDI/daq_server_deployment/tree/master/ansible

.. _pco setup script : https://git.psi.ch/HPDI/daq_server_deployment/blob/master/ansible/roles/generic_service/templates/pco_writer_setup.sh
.. _conda exported file: https://git.psi.ch/hpdi_configs/sls/tree/master/hosts/xbl-daq-32

.. _python flask server : https://flask.palletsprojects.com/en/1.1.x/
.. _pco_rclient : https://github.com/paulscherrerinstitute/pco_rclient

.. _PSI conda channel : https://anaconda.org/paulscherrerinstitute/pco_rclient
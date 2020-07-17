Welcome to TOMCAT pco writer documentation
=======================================

.. meta::
   :description lang=en: This library is used for creating C++ based stream writer for H5 files. It focuses on the functionality and performance needed for high performance detectors integrations and pco.edge cameras.


This library is used for creating C++ based stream writer for H5 files. It focuses on the functionality and performance needed for high performance detectors integrations and pco.edge cameras.

Key features of the library: 
      * Get data from ZMQ stream (Array-1.0 protocol) - `htypes specification`_
      * Write Nexus complaint H5 file (User specified) - `nexus format`_
      * Specify additional zmq stream parameters to write to file.
      * Receive additional parameters to write to file via REST api.
      * Interaction with the writer over REST api (kill, stop, statistics, parameters).
      

In addition, the TOMCAT pco writer:
      * Offers a remote client for controlling and spawning the writer - `pco_rclient`_
      * Uses of a configuration file that specifies key parameters for each execution of the writer.
      * Runs on xbl-daq-32 and is controlled remotely using the pco_rclient (this machine is inside tomcat's gateway - x06sa-gw).
      * Is deployed/installed using ansible playbooks in a conda environment. Check `ansible configuration`_ and `conda env`_. 

It is developed using c++ on an open-source project (`github repository`_) and documented here using readthedocs.

.. _htypes specification : https://github.com/paulscherrerinstitute/htypes
.. _nexus format : https://www.nexusformat.org/
.. _pco_rclient : https://github.com/paulscherrerinstitute/pco_rclient
.. _ansible configuration : https://git.psi.ch/HPDI/daq_server_deployment/tree/master/ansible
.. _conda env : https://git.psi.ch/hpdi_configs/sls
.. _github repository : https://github.com/paulscherrerinstitute/lib_cpp_h5_writer/tree/tomcat/

Contents:

.. toctree::
    :glob:
    :maxdepth: 3

    installation
    getting-started
    usage
    development
    contact

Welcome to TOMCAT PCO writer documentation
==========================================

.. meta::
   :description lang=en: This library is used for creating C++ based stream writer for H5 files. It focuses on the functionality and performance needed for high performance detectors integrations and pco.edge cameras.


This library is used for creating C++ based stream writer for H5 files. It focuses on the functionality and performance needed for high performance detectors integrations and pco.edge cameras.

Key features of the library: 
      * Get data from ZMQ stream (Array-1.0 protocol) - `htypes specification`_
      * Write Nexus complaint H5 file (User specified) - `nexus format`_
      * Specify additional zmq stream parameters to write to file.
      * Receive additional parameters to write to file via REST api.
      * Interaction with the writer over REST api (status, kill, stop, statistics, parameters).


In addition, the TOMCAT pco writer:
      * Offers a remote client for controlling and spawning the writer - `pco_rclient`_
      * Runs on xbl-daq-34 and is controlled remotely using the `pco_rclient`_ (inside tomcat's gateway - x02da-gw).
      * The `pco_rclient`_ allows to fully control automated acquisitions and the usage of scripting 
      * Is deployed/installed using ansible playbooks in a conda environment. Check `ansible configuration`_ and `conda env`_. 
      * It's distributed via Anaconda and easily installed using the `conda_client`_

It is developed using c++ on an open-source project (`github repository`_) and documented here using readthedocs.

.. _htypes specification : https://github.com/paulscherrerinstitute/htypes
.. _nexus format : https://www.nexusformat.org/
.. _pco_rclient : https://github.com/paulscherrerinstitute/pco_rclient
.. _ansible configuration : https://git.psi.ch/HPDI/daq_server_deployment/tree/master/ansible
.. _conda env : https://git.psi.ch/hpdi_configs/sls
.. _github repository : https://github.com/paulscherrerinstitute/lib_cpp_h5_writer/tree/tomcat/
.. _conda_client : https://anaconda.org/paulscherrerinstitute/pco_rclient

Architecture Diagram
--------------------
.. image:: https://github.com/paulscherrerinstitute/lib_cpp_h5_writer/raw/tomcat/docs/pco_diagram.jpg
  :width: 100%
  :alt: Architecture Diagram

Useful links / badges
---------------------

H5 Writer:
|codefactor| |docker_build| |release| |release_date| |languages|

    * `lib_cpp_h5_writer repo <https://github.com/paulscherrerinstitute/lib_cpp_h5_writer/tree/tomcat>`_
    * `Ansible playbook <https://git.psi.ch/HPDI/daq_server_deployment/tree/master/ansible>`_
    * `Code factor lib_cpp_h5_writer <https://www.codefactor.io/repository/github/paulscherrerinstitute/lib_cpp_h5_writer>`_
    * `Docker hub image <https://hub.docker.com/repository/docker/lhdamiani/tomcat_writer>`_

H5 Writer Client:
|codefactor_client| |release_client| |release_date_client| |languages_client|

    * `pco_rclient repo <https://anaconda.org/paulscherrerinstitute/pco_rclient>`_
    * `Conda pco_rclient package <https://anaconda.org/paulscherrerinstitute/pco_rclient>`_



.. |languages| image:: https://img.shields.io/github/languages/top/paulscherrerinstitute/lib_cpp_h5_writer
    :alt: Top languages lib_cpp_h5_writer

.. |languages_client| image:: https://img.shields.io/github/languages/top/paulscherrerinstitute/pco_rclient
    :alt: Top languages pco_rclient

.. |release_date| image:: https://img.shields.io/github/release-date/paulscherrerinstitute/lib_cpp_h5_writer
    :alt: Release date lib_cpp_h5_writer

.. |release_date_client| image:: https://img.shields.io/github/release-date/paulscherrerinstitute/pco_rclient
    :alt: Release date pco_rclient

.. |release| image:: https://img.shields.io/github/v/release/paulscherrerinstitute/lib_cpp_h5_writer
    :alt: Release date lib_cpp_h5_writer

.. |release_client| image:: https://img.shields.io/github/v/release/paulscherrerinstitute/pco_rclient
    :alt: Release date pco_rclient


.. |docker_build| image:: https://img.shields.io/docker/cloud/build/lhdamiani/tomcat_writer.svg
    :alt: Docker build
    :target: https://hub.docker.com/repository/docker/lhdamiani/tomcat_writer

.. |codefactor| image:: https://www.codefactor.io/repository/github/paulscherrerinstitute/lib_cpp_h5_writer/badge
    :alt: CodeFactor lib_cpp_h5_writer
    :target: https://www.codefactor.io/repository/github/paulscherrerinstitute/lib_cpp_h5_writer

.. |codefactor_client| image:: https://www.codefactor.io/repository/github/paulscherrerinstitute/pco_rclient/badge
    :alt: CodeFactor lib_cpp_h5_writer
    :target: https://www.codefactor.io/repository/github/paulscherrerinstitute/pco_rclient


Useful info
^^^^^^^^^^^

(sudo required) To control the server from xbl-daq-34: 
   * ``systemctl start pco_writer-pco{1-2}``
   * ``systemctl stop pco_writer-pco{1-2}``
   * ``systemctl status pco_writer-pco{1-2}``
            
    
(sudo required) To check the server's log from xbl-daq-34:
    * ``journalctl -u pco_writer-pco{1-2} -f``

.. note::
   journalctl allows the usage of, for example: **--since "1 hour ago"** to apply filters on the output file. For more instructions check `journalct guide`_.

.. _journalct guide: https://www.loggly.com/ultimate-guide/using-journalctl/


The ```pco_writer-pco{1-2}``` logs are shipped to PSI `kibana`_ infrastructure using `journalbeat`_. After logging in, locate the dashboard: ```[xbl-daq-34] Tomcat PCO Writer``` or use the `direct`_ link. 

.. note::
   The search bar inside the dashboard uses the Kibana Query Language `(KQL)`_. It offers a simplified query syntax and support for scripted fields.
 
.. _kibana : http://kibana.psi.ch/
.. _journalbeat : https://www.elastic.co/guide/en/beats/journalbeat/current/index.html
.. _direct : https://kibana.psi.ch/s/gfa/app/dashboards#/view/68e8b3d0-5b0f-11eb-a4fd-4fdac4bb5369?_g=(filters%3A!()%2CrefreshInterval%3A(pause%3A!t%2Cvalue%3A0)%2Ctime%3A(from%3Anow-15m%2Cto%3Anow))
.. _(KQL) : https://www.elastic.co/guide/en/kibana/7.9/kuery-query.html
Contents:

.. toctree::
    :glob:
    :maxdepth: 2

    index
    installation
    getting-started
    usage
    pco_rclient-api
    flaskserver-api
    development
    contact

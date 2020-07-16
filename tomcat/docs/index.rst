Welcome to TOMCAT pco writer documentation
=======================================

.. meta::
   :description lang=en: TOMCAT pco writer is a tool to receive the zmq stream coming from the PCO camera and save it into H5 file.


TOMCAT pco writer is a tool to receive the zmq stream coming from the PCO camera and save it into H5 file.

TOMCAT pco writer:

      * Based on a configuration file, TOMCAT pco writer will receive a zmq stream that is sent from the pco camera and store it into a H5 file.
      * The writer runs on xbl-daq-32 and is controlled by the pco_rclient remotely on any console or machine within tomcat's gateway (x06sa-gw).

It is developed using `c++`_ on an open-source project (`github repository`_) and documented here using readthedocs.

.. _python : https://www.cplusplus.com/
.. _github repository : https://github.com/paulscherrerinstitute/lib_cpp_h5_writer/tree/tomcat/

Contents:

.. toctree::
    :glob:
    :maxdepth: 3

    installation
    getting-started
    usage
    development
    faq
    contact

#####
Usage
#####

TOMCAT PCO Writer client usage
------------------------------

To start an acquisition use:

.. code-block:: bash
    
    $ pco_rclient start <path/to/config/file.pco>


To stop/kill/status use:

.. code-block:: bash
    
    $ pco_rclient <stop/kill/status>


TOMCAT PCO Writer log
---------------------

To verify the log of the writer:

.. code-block:: bash
    
    $ journalctl -u pco_writer_1 -f


.. note::
   journalctl allows  the usage of, for example: **--since "1 hour ago"** to apply filters on the output file. For more instructions check `journalct guide`_.

.. _journalct guide:  https://www.loggly.com/ultimate-guide/using-journalctl/
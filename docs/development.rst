###########
Development
###########

Useful info
-----------

+------------+------------+
| Address:Port   | Description |
+============+============+
| tcp://129.129.99.104:8080 | PCO camera (1G copper link on x02da-pco-4) |
+------------+------------+
| http://xbl-daq-32:9901 | Flask python server |
+------------+------------+
| http://xbl-daq-32:9955 | Writer API address (only valid during an acquisition)  |
+------------+------------+
| tcp://pc9808:9999 | DEBUG PCO Camera |
+------------+------------+


Contribute and create a merge request
-------------------------------------
You can create a new branch to implemenet new features for the tomcat pco writer by doing ::

    $ git clone https://github.com/paulscherrerinstitute/lib_cpp_h5_writer.git
    $ git fetch 
    $ git checkout tomcat
    $ git pull
    $ git checkout -b <new_branch>

For the PCO_RCLIENT project: 

    $ git clone https://github.com/paulscherrerinstitute/pco_rclient.git
    $ git fetch 
    $ git pull
    $ git checkout -b <new_branch>
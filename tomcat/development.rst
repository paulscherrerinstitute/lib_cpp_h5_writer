###########
Development
###########

Tests
-----

(Preliminary) Unit tests are available on the folder `tests`. To run the unit tests, use the command::

    $ python -m unittest gitutils/tests/gitutils_test.py

Installation for distribution
-----------------------------
The package has to be installed as root on ``gfalcd.psi.ch``::

    # Sourcing the python
    $ source /opt/gfa/python
    # Installing the gitutils package
    $ conda install -c paulscherrerinstitute gitutils

As this will change the global Python distribution, make sure that only the gitutils package gets updated.

Building the conda package
--------------------------
First, login into ```gfa-lc6-64```, source the right anaconda environment by executing the command::

    $ source /opt/gfa/python


After that, clone into the repository or pull the latest changes (if you've already cloned it before). The package can be build using::

    $ conda build conda-recipe

.. note:: Remember to increase the package version before the build (inside `setup.py` and `conda-recipe/meta.yaml`)

After building, the package should be uploaded to anaconda.org via the command displayed at the end of the build process, as in::

    $ anaconda -t <PERSONAL_CONDA_TOKEN> upload /afs/psi.ch/user/<PATH_TO_USER>/conda-bld/linux-64/<PACKAGE_NAME>

.. note::  If you need to build for different python versions, use the command (where X.X is the specific needed version of python)::

    $ conda build conda-recipe --python=X.X

Contribute and create a merge request
-------------------------------------
Alternatively, you can fork gitutils::

    $ gitutils fork controls_highlevel_applications/gitutils

Develop new feature(s) on your personal fork, and submit a merge request::

    # From the home directory of your fork
    $ gitutils merge -t <new_feature_merge_title> -d <description_of_new_feature>

If the merge request is approved, it will be integrated and the conda package will be updated.

Coding guidelines
-----------------
.. todo:: Introduce gitutils coding guidelines.

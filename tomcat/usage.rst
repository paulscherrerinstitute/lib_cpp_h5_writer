#####
Usage
#####

Gitutils
--------

.. code-block:: bash

      usage: gitutils [-h] [-e ENDPOINT] {fork,merge} ...

      GITUTILS is a tool to facilitate the server-side operations when developing software that uses git repositories.

      optional arguments:
      -h, --help            show this help message and exit
      -e ENDPOINT, --endpoint ENDPOINT
                              Endpoint of the git server. Default: https://git.psi.ch

      command:
      valid commands

      {fork,merge}          commands
      fork                Creates a fork from the repository.
      merge               Creates a request to merge the defined fork to the original repository.
      login               Fetches the token for the usage of gitutils and stores it on the user's home directory file (~/.gitutils_token).

.. note:: To see the gitutils usage help, you can use::

      $ gitutils -h

Fork
----

.. code-block:: bash

      usage: gitutils fork [-h] [-n] [-c] project

      positional arguments:
      project         (REQUIRED) Indicates the project to be forked. It can be of three different formats:
                        - https://git.psi.ch/<group_name>/<project_name> : The user provides
                        the direct http to the git repository.
                        - <group_name>/<project_name> : The user provides a combination of
                        group_name and project_name divided by "/".
                        - <project_name> : The user provides the name of the project name.
                        Gitutils will fetch the name of the group (keep in mind, that this may
                        cause ambiguity problems).

      optional arguments:
      -h, --help      show this help message and exit
      -n, --no_clone  Indicates that the forked project will not be cloned after forking. A fork
                        will be created on the server-side and no clone nor upstream will be
                        generated on the local git server.
      -c, --clean     Indicates to delete any existing fork project under your personal group.
                        This might be necessary to fork and clone into a clean copy of the original
                        repository. The desired forked project must not be a pre-existing forked
                        project under your personal projects.

.. note:: To see the fork usage help, you can use::

      $ gitutils fork -h

Merge
-----

.. code-block:: bash


      usage: gitutils merge [-h] [-t TITLE] [-p PROJECT] [-d DESCRIPTION]

      optional arguments:
      -h, --help            show this help message and exit
      -t TITLE, --title TITLE
                              The title of the merge request that is going to be created.
      -p PROJECT, --project PROJECT
                              Indicates the project to be forked. It can be of four different formats:
                              - "" : (DEFAULT) The user doesn't provide this argument, the project's group and name
                                    will be fetched from the /.git/config folder within the path where the
                                    gitutils is being called.
                              - https://git.psi.ch/<group_name>/<project_name> : The user provides the direct
                                    http to the git repository.
                              - <group_name>/<project_name> : The user provides a combination of group_name and
                                    project_name divided by "/".
                              - <project_name> : The user provides the name of the project name. Gitutils will
                                    fetch the name of the group (keep in mind, that this may cause ambiguity
                                    problems).
      -d DESCRIPTION, --description DESCRIPTION
                              The description of the merge request that is going to be created.

.. note:: To see the merge usage help, you can use::

      $ gitutils merge -h
#############################
Getting started with Gitutils
#############################

It is assumed that the gitutils' user is familiar with basic git commands (as add, commit, push and pull). If not, we refer to in `atlassian tutorial`_ to learn the basics.

.. _atlassian tutorial : https://www.atlassian.com/git/tutorials

Before any command, a token will be fetched to validate the access to the gitlab server. If the user wants to create the token without any specific command, one can use::

    $ gitutils login

A file (~/.gitutils_token) will be created on the home directory and it will store the token. 

Fork Walk-through
-----------------
1. Define a project to fork and issue the command. Once a repository is forked, it also creates a local clone and an upstream link to the reference repository. Arguments: **-p**, **-n**, **-c**. Examples:

    - To fork and clone into a repository, use the following command::

        $ gitutils fork <group_name>/<repository_name>

    - To fork and not clone, add the directive **-n** at the end of the previous command, as in::

            $ gitutils fork <group_name>/<repository_name> -n

    - To delete existing fork and create a clean fork of a repository, use the following command::

            $ gitutils fork <group_name>/<repository_name> -c

    - To fork (using the full path), clean existing fork and not clone an existing repository::

            $ gitutils fork https://git.psi.ch/<group_name>/<repository_name> -n -c

2. Implement the changes/development necessary on the forked repository.
3. Add all changes, commit and push the changes to your forked repository using git command line, as in::

    $ git add .
    $ git commit -m <commit_message>
    $ git push


.. note:: When a successful fork happens, it already creates the upstream link. This is done automatically. Therefore, to synchronize your fork with the current state of the original repository and deal with possible merge conflicts, do the following::

    $ git fetch upstream
    $ git merge upstream/master

Merge Walk-through
------------------
1. Once all the necessary changes/development have been commited and pushed to a forked repository.
2. Navigate to the home folder of your forked repository (where the ``/.git`` folder is). Issue the command to merge. Arguments:**-t**, **-d**, **-p**.
    - To create a merge request for a repository, use the following command while on a git repository folder::

        $ gitutils merge -t <title> -d <description>

    - To create a merge request for a repository by using the argument **-p** to indicate the project::

        $ gitutils merge -p <group_name>/<repository_name> -t <title> -d <description>

    - To create a merge request indicating the full-path to the repository and without giving a description::

        $ gitutils merge -p https://git.psi.ch/<group_name>/<repository_name> -t <title>

.. note:: Gitutils will assume the command is being executed on the git repository folder. Alternatively, one can use the directive **-p** to indicate directly which project should be merged. If title or description are not provided by the user, default values are going to be used.

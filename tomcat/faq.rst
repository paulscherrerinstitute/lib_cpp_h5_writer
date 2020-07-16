###
FAQ
###

Gitutils can't find a group/project...
    Gitutils have access only to the groups/projects that your token allows
    it to access. If your permissions are not valid to access certain groups,
    even though they might exist you will not be able to fork or merge.

I cannot fork the requested repository...
    It is likely that you need to specify the group and project name in the
    following format::

        $ gitutils fork <group_name>/<project_name>

It refuses to merge saying that it is not a fork...
    Make sure when you issue a merge using gitutils that you point to the
    forked repository. Gitutils will get the original repository (in which
    the merge request is going to be created) from the properties of the forked
    repository.

    Use ::

        $ gitutils merge <personal_fork>/<project_name>

    Instead of::

        $ gitutils merge <original_project>/<project_name>

Why it doesn't asks for authentication?
    At the first run of ``gitutils`` on your local machine, ``gitutils``
    creates a file on your home folder ``~/.gitutils_token`` and stores
    your personal token there. It will be continuously used until it is
    not valid anymore.

If you have any other question, please let us know.

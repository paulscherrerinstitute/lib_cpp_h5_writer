Welcome to Gitutils documentation
=======================================

.. meta::
   :description lang=en: Gitutils is a tool to facilitate the server-side operations when developing software that uses git repositories. It allows users to create forks and merge requests directly from the command line interface.


Gitutils is a tool to facilitate the server-side operations when developing software that uses git repositories. It allows users to create forks and merge requests directly from the command line interface.

Gitutils allows you to:

      * **Fork** existing repositories into your own personal space. Doing a fork is strongly recommended to freely experiment your changes and/or development in a safe working space without affecting the original project.
      * **Merge** existing repositories from your personal space into the original repository. Merge has the purpose of joining different repositories to incorporate changes into repository.

It is developed using `python`_ on an open-source project (`github repository`_), distributed using anaconda via the Paul Scherrer Institute channel (`anaconda channel`_) and documented here using readthedocs.
Gitutils authenticates on the git server using the OAuth2 protocol. If the token is non-existant or not valid, gitutils will request username and password and store the token in a file located on the user's home directory called ``.gitutils_token``. The user will not be requested for username nor password until the saved token is not valid anymore.

.. _python : https://www.python.org/
.. _github repository : https://git.psi.ch/controls_highlevel_applications/gitutils
.. _anaconda channel : https://anaconda.org/paulscherrerinstitute/gitutils

Contents:

.. toctree::
    :glob:
    :maxdepth: 3

    installation
    getting-started
    usage
    development
    faq
    sheet
    changelog
    contact

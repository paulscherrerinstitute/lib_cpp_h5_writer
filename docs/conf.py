# -*- coding: utf-8 -*-

import sys, os

sys.path.insert(0, os.path.abspath('.'))

extensions = ['sphinx.ext.autodoc', 'sphinx.ext.doctest', 'sphinx.ext.todo', 'sphinx.ext.napoleon',
              'sphinx.ext.coverage', 'sphinx.ext.ifconfig']

todo_include_todos = True
templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
exclude_patterns = []
add_function_parentheses = True
add_module_names = True
pygments_style = 'sphinx'

# A list of ignored prefixes for module index sorting.
#modindex_common_prefix = []

project = u'Tomcat PCO Writer'

version = '0.1.0'
release = ''

# -- Options for HTML output ---------------------------------------------------

html_title = "Tomcat PCO Writer"
html_static_path = ['_static']
html_domain_indices = False
html_use_index = False
html_show_sphinx = False
htmlhelp_basename = 'pco_camera'
html_show_sourcelink = False

html_theme_options = {
    'display_version': False}


# -- Options for Code Examples output ---------------------------------------------------

code_example_dir = "code-example"

################################################################################


def setup(app):
     from sphinx.util.texescape import tex_replacements
     tex_replacements += [(u'♮', u'$\\natural$'),
                          (u'ē', u'\=e'),
                          (u'♩', u'\quarternote'),
                          (u'↑', u'$\\uparrow$'),
                          ]

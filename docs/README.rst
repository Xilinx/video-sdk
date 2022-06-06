###############################################################
Editing and Building the Documentation
###############################################################


****************************************
Prerequisites
****************************************

Building the HTML documentation requires Sphinx and some of its extensions. 
These dependencies must be installed as shown below. 
This requires a Python 3 installation.

::

  pip3 install -U sphinx
  pip3 install recommonmark
  pip3 install sphinx-copybutton
  pip3 install sphinx-markdown-tables
  pip3 install rst2pdf


****************************************
General Workflow
****************************************

#. Make the desired doc edits under the ``docs`` folder
#. Generate the HTML documentation using Sphinx::

    make clean html

#. Open the documentation in a web browser::

    firefox ./_build/html/index.html &

#. Inspect the generated HTML in your web browser to check if it meets expectations
#. When appropriate, submit a pull request to the repo


****************************************
Details about the Sphinx Setup
****************************************

The ``conf.py`` file contains the settings of the Sphinx project. Noteworthy project settings include:

- The version and release information

- The master doc is set to ``index.rst``

- Sphinx will only process .rst file, not .md

- The ``|SDK|`` macro will be automatically replaced by ``Xilinx Video SDK`` in all .rst files

- ``html_show_sourcelink`` is set to False, which prevents the generation of the "View page source" link in the HTML pages

- The ``exclude_patterns`` option is used to exclude certain directories from processing by Sphinx. Files which are included in other .rst files (with the ``.. include::`` directive) should be placed in excluded directories so that they don't get processed as standalone files::

	exclude_patterns = ['README.rst', 'include', 'examples/xma/include']

- The `sphinx.ext.extlinks extension <https://www.sphinx-doc.org/en/master/usage/extensions/extlinks.html>`_ is enabled, and the ``extlink`` below is defined. This provides a convenient shortcut to the URL of the repo, which can be globally changed by editing the conf.py file::

	extlinks = {
	    'url_to_repo': ('https://github.com/Xilinx/video-sdk/blob/master/%s', ''),
	}




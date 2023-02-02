###############################################################
Editing and Building the Documentation
###############################################################


****************************************
Overview
****************************************

The .rst sources for the documentation are located in the ``sources`` directory of the ``gh-pages`` branch of this repository. The ``sources`` directory contains subdirectories (``vX.Y``) corresponding to each release of the SDK. Each of these subdirectories contains the documentation sources for that particular release.

The documentation is generated using a Makefile located in the ``build`` directory at the root of the ``gh-pages`` branch.

The HTML output for the sources in a particular ``gh-pages/sources/vX.Y`` directory is placed in a corresponding ``vX.Y`` directory at the root of the ``gh-pages`` branch.


****************************************
Prerequisites
****************************************

Building the HTML documentation requires Sphinx and some of its extensions. These dependencies must be installed as shown below. This requires a Python 3 installation.

::

  pip3 install -U sphinx
  pip3 install recommonmark
  pip3 install sphinx-copybutton
  pip3 install sphinx-markdown-tables
  pip3 install rst2pdf


****************************************
General Workflow
****************************************

#. Make your edits in the desired ``sources/vX.Y`` directory
#. From the root of the cloned branch, generate the HTML documentation using Sphinx::

    make -C build all

#. Open the generated HTML documentation in a web browser::

    firefox ./index.html &

#. Inspect the generated HTML in your web browser to check if it meets expectations
#. When appropriate, submit a pull request to the ``gh-pages```branch of the ``video-sdk`` repository


****************************************
Details about the Sphinx Setup
****************************************

The ``conf.py`` file contains the settings of the Sphinx project. Noteworthy project settings include:

- The version and release information

- The master doc is set to ``index.rst``

- Onyl .rst files are processes, files with the .md extension are ignored

- The ``|SDK|`` macro will be automatically replaced by ``Xilinx Video SDK`` in all .rst files

- ``html_show_sourcelink`` is set to False, which prevents the generation of the "View page source" link in the HTML pages

- The ``exclude_patterns`` option is used to exclude certain directories from processing by Sphinx. Files which are included in other .rst files (with the ``.. include::`` directive) should be placed in excluded directories so that they don't get processed as standalone files::

	exclude_patterns = ['README.rst', 'include', 'examples/xma/include']

- The `sphinx.ext.extlinks extension <https://www.sphinx-doc.org/en/master/usage/extensions/extlinks.html>`_ is enabled, and various ``extlinks`` are defined, providing convenient URL shortcuts usable in the RST sources.


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

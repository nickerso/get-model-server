.. _get-model-server-build:

============================================================
General Epithelial Transport Model Server Build Instructions
============================================================

This document describes how to build the General Epithelial Transport (GET) Model Server (GET-MS).

GET-MS uses the CMake cross platform build tool to generate platform build files.

GET-MS has seven dependencies that are required, they are:

  Xml2
  MicroHttpd
  RDF
  Raptor
  Rasqal
  CSim
  SEDML (https://github.com/fbergmann/libSEDML)

These libraries must be available on the build platform.

Building SEDML from Source
==========================

To build the C++ SEDML library the SBML library is required, (http://sourceforge.net/projects/sbml/files/libsbml/5.8.0/stable/).  CMake can be used to configure the SBML library.  The default configuration is suitable.  CMake can be used to configure the SEDML library.  Set the LIBSEDML_DEPENDENCY_DIR configuration variable to the location of the SBML install root if it has been installed in a non-standard location.

Configuring GET-MS
==================

Use ccmake or cmake-gui to configure GET-MS.  GET-MS should be configured as an out of source build, using sibling directories for the source and build directories works best for maximum compatibility with the major developement environments.  Configure GET-MS as desired.


Documentation
=============

The documentation can be built using Sphinx.  Install Sphinx to enable the 'docs' target then issue the command::

  make docs



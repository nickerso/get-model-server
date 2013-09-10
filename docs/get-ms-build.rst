.. _get-model-server-build:

================================================================
Generalised Epithelial Transport Model Server Build Instructions
================================================================

This document describes how to build the General Epithelial Transport (GET) Model Server (GET-MS).  GET-MS has only successfully been built on OSX and GNU/Linux based platforms, it should be possible to build on Windows platforms also.

GET-MS uses the CMake cross platform build tool to generate platform build files.

GET-MS has seven dependencies that are required, they are:

  * Xml2
  * MicroHttpd
  * RDF
  * Raptor
  * Rasqal
  * CSim (googlecode aka CellML-Simulator)
  * SEDML (https://github.com/fbergmann/libSEDML)

These libraries must be available on the build platform.  Raptor and Rasqal are dependencies of the Redland RDF library.  But direct access to Raptor is required, though not wanted.

Building SEDML from Source
==========================

To build the C++ SEDML library the SBML library is required, (http://sourceforge.net/projects/sbml/files/libsbml/5.8.0/stable/).  CMake can be used to configure the SBML library.  Configure the SBML library to have::

  WITH_LIBXML ON
  WITH_ZLIB ON

and all other WITH_* variables to OFF.

CMake can be used to configure the SEDML library.  Set the LIBSEDML_DEPENDENCY_DIR configuration variable to the location of the SBML install root if it has been installed in a non-standard location.

Configuring GET-MS
==================

Use ccmake or cmake-gui to configure GET-MS.  GET-MS should be configured as an out of source build, using sibling directories for the source and build directories works best for maximum compatibility with the major developement environments.  Configure GET-MS as desired.

Run
===

The GET-MS is run with the following command::

  ./gms 8888 GMS

Documentation
=============

The documentation can be built using Sphinx.  Install Sphinx to enable the 'docs' target then issue the command::

  make docs



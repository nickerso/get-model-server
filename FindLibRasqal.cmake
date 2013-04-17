# - Try to find librasqal
# Once done, this will define
#
#  librasqal_FOUND - system has librasqal
#  librasqal_INCLUDE_DIRS - the librasqal include directories
#  librasqal_LIBRARIES - link these to use librasqal

include(LibFindMacros)

# Dependencies
#libfind_package(Magick++ Magick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(librasqal_PKGCONF rasqal)

# Include dir
find_path(librasqal_INCLUDE_DIR
  NAMES rasqal.h
  PATHS ${librasqal_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(librasqal_LIBRARY
  NAMES rasqal
  HINTS ${librasqal_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(librasqal_PROCESS_INCLUDES librasqal_INCLUDE_DIR)
set(librasqal_PROCESS_LIBS librasqal_LIBRARY)
libfind_process(librasqal)

## WHY doesn't the pkg-config pick this up!?!
#set(librdf_INCLUDE_DIRS
#  /usr/local/Cellar/redland/1.0.15/include
#  /usr/local/Cellar/raptor/2.0.7/include/raptor2
#  /usr/local/Cellar/rasqal/0.9.29/include/rasqal)

#set(librdf_LIBRARIES -L/usr/local/Cellar/redland/1.0.15/lib -lrdf)

#message(STATUS "librdf_INCLUDE_DIRS = ${librdf_INCLUDE_DIRS}")
#message(STATUS "librdf_LIBRARIES = ${librdf_LIBRARIES}")

# - Try to find libraptor
# Once done, this will define
#
#  libraptor_FOUND - system has libraptor
#  libraptor_INCLUDE_DIRS - the libraptor include directories
#  libraptor_LIBRARIES - link these to use libraptor

include(LibFindMacros)

# Dependencies
#libfind_package(Magick++ Magick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(libraptor_PKGCONF raptor2)

# Include dir
find_path(libraptor_INCLUDE_DIR
  NAMES raptor2.h
  PATHS ${libraptor_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(libraptor_LIBRARY
  NAMES raptor2
  HINTS ${libraptor_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(libraptor_PROCESS_INCLUDES libraptor_INCLUDE_DIR)
set(libraptor_PROCESS_LIBS libraptor_LIBRARY)
libfind_process(libraptor)

## WHY doesn't the pkg-config pick this up!?!
#set(librdf_INCLUDE_DIRS
#  /usr/local/Cellar/redland/1.0.15/include
#  /usr/local/Cellar/raptor/2.0.7/include/raptor2
#  /usr/local/Cellar/rasqal/0.9.29/include/rasqal)

#set(librdf_LIBRARIES -L/usr/local/Cellar/redland/1.0.15/lib -lrdf)

#message(STATUS "librdf_INCLUDE_DIRS = ${librdf_INCLUDE_DIRS}")
#message(STATUS "librdf_LIBRARIES = ${librdf_LIBRARIES}")

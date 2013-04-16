# - Try to find librdf
# Once done, this will define
#
#  librdf_FOUND - system has librdf
#  librdf_INCLUDE_DIRS - the librdf include directories
#  librdf_LIBRARIES - link these to use librdf

include(LibFindMacros)

# Dependencies
#libfind_package(Magick++ Magick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(librdf_PKGCONF redland)

# Include dir
find_path(librdf_INCLUDE_DIR
  NAMES redland.h
  PATHS ${librdf_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(librdf_LIBRARY
  NAMES rdf
  HINTS ${librdf_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(librdf_PROCESS_INCLUDES librdf_PKGCONF_INCLUDE_DIRS)
set(librdf_PROCESS_LIBS librdf_LIBRARY)
libfind_process(librdf)

## WHY doesn't the pkg-config pick this up!?!
#set(librdf_INCLUDE_DIRS
#  /usr/local/Cellar/redland/1.0.15/include
#  /usr/local/Cellar/raptor/2.0.7/include/raptor2
#  /usr/local/Cellar/rasqal/0.9.29/include/rasqal)

#set(librdf_LIBRARIES -L/usr/local/Cellar/redland/1.0.15/lib -lrdf)

message(STATUS "librdf_INCLUDE_DIRS = ${librdf_INCLUDE_DIRS}")
message(STATUS "librdf_LIBRARIES = ${librdf_LIBRARIES}")

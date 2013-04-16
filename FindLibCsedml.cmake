# - Try to find clibsedml
# Once done, this will define
#
#  clibsedml_FOUND - system has libmicrohttpd
#  clibsedml_INCLUDE_DIRS - the libmicrohttpd include directories
#  clibsedml_LIBRARIES - link these to use libmicrohttpd

include(LibFindMacros)

# Dependencies
#libfind_package(Magick++ Magick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(clibsedml_PKGCONF clibsedml)

# Include dir
find_path(clibsedml_INCLUDE_DIR
  NAMES sedml/document.h
  PATHS ${clibsedml_PKGCONF_INCLUDE_DIRS} /Users/dnic019/shared-folders/resources/my-libs/clibsedml/mac-osx/include
)

# Finally the library itself
find_library(clibsedml_LIBRARY
  NAMES sedml
  PATHS ${clibsedml_PKGCONF_LIBRARY_DIRS} /Users/dnic019/shared-folders/resources/my-libs/clibsedml/mac-osx/libs
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(clibsedml_PROCESS_INCLUDES clibsedml_INCLUDE_DIR)
set(clibsedml_PROCESS_LIBS clibsedml_LIBRARY)
libfind_process(clibsedml)

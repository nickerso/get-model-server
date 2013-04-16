# - Try to find libcsim
# Once done, this will define
#
#  libcsim_FOUND - system has libmicrohttpd
#  libcsim_INCLUDE_DIRS - the libmicrohttpd include directories
#  libcsim_LIBRARIES - link these to use libmicrohttpd

include(LibFindMacros)

# Dependencies
#libfind_package(Magick++ Magick)

# Include dir
find_path(libcsim_INCLUDE_DIR
  NAMES CellmlSimulator.hpp
  PATHS /Users/dnic019/shared-folders/resources/my-libs/CSim/mac-osx/include
)

# Finally the library itself
find_library(libcsim_LIBRARY
  NAMES csim
  PATHS /Users/dnic019/shared-folders/resources/my-libs/CSim/mac-osx/lib
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(libcsim_PROCESS_INCLUDES libcsim_INCLUDE_DIR)
set(libcsim_PROCESS_LIBS libcsim_LIBRARY)
libfind_process(libcsim)

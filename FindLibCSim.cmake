# - Try to find libcsim
# Once done, this will define
#
#  LIBCSIM_FOUND - system has libcsim
#  LIBCSIM_INCLUDE_DIR - the libcsim include directories
#  LIBCSIM_LIBRARIES - link these to use libcsim
#  LIBCSIM_PLUGINS - the extra libraries that need to be installed for applications making use of libcsim

# Include dir
find_path(LIBCSIM_INCLUDE_DIR CellmlSimulator.hpp)

# Finally the library itself
find_library(LIBCSIM_LIBRARY csim)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibCSim LIBCSIM_LIBRARY LIBCSIM_INCLUDE_DIR)

if(LIBCSIM_FOUND)
  SET(LIBCSIM_INCLUDE_DIRS ${LIBCSIM_INCLUDE_DIR})
  SET(LIBCSIM_LIBRARIES ${LIBCSIM_LIBRARY})

  find_program(CSIM_APPLICATION csim DOC "The location of the CSim application.")
  if(EXISTS ${CSIM_APPLICATION})
    get_filename_component(CSIM_PLUGIN_DIR ${CSIM_APPLICATION} PATH)
    file(GLOB CSIM_PLUGINS "${CSIM_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    message(STATUS "CSIM plugins: ${CSIM_PLUGINS}")
  elseif(EXISTS ${CSIM_APPLICATION})
    MESSAGE(ERROR "Unable to find the CSim application")
  endif(EXISTS ${CSIM_APPLICATION})
endif(LIBCSIM_FOUND)

mark_as_advanced(LIBCSIM_LIBRARY LIBCSIM_INCLUDE_DIR)



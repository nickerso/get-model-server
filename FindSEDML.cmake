# - Try to find SEDML
# Once done, this will define
#
#  SEDML_FOUND        - sedml was found
#  SEDML_INCLUDE_DIRS - the sedml include directories
#  SEDML_LIBRARIES    - link these to use sedml

# Include dir
find_path(SEDML_INCLUDE_DIR
  NAMES sedml/SedTypes.h
)

# Finally the library itself
find_library(SEDML_LIBRARY
  NAMES ${SEDML_NAMES} sedml
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SEDML SEDML_LIBRARY SEDML_INCLUDE_DIR)

if(SEDML_FOUND)
  set(SEDML_INCLUDE_DIRS ${SEDML_INCLUDE_DIR})
  set(SEDML_LIBRARIES ${SEDML_LIBRARY})
endif(SEDML_FOUND)

mark_as_advanced(SEDML_LIBRARY SEDML_INCLUDE_DIR)


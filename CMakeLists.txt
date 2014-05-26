# General Epithelial Transport - model server
project(get-model-server)

cmake_minimum_required(VERSION 2.8)

# Add in the directory with the FindCellML module
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

SET(CMAKE_FIND_LIBRARY_SUFFIXES_ORIGINAL ${CMAKE_FIND_LIBRARY_SUFFIXES})

SET(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_FIND_LIBRARY_SUFFIXES})
FIND_PACKAGE(LibMicroHttpd REQUIRED)
FIND_PACKAGE(LibRDF REQUIRED)
FIND_PACKAGE(LibRaptor REQUIRED)
FIND_PACKAGE(LibRasqal REQUIRED)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_ORIGINAL})
FIND_PACKAGE(LibCSim REQUIRED)
FIND_PACKAGE(LibSBML REQUIRED)
FIND_PACKAGE(SEDML REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED QUIET)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_LIBS "curl" "xml2")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_LIBS "curl" "xml2" "pthread" "gcrypt" "gnutls" "ltdl" "dl" "db-5.3" "pcre" "mhash" "uuid" "m" "gmp" "xslt" "yajl")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Set version information in a config.h file
set(GMS_VERSION_MAJOR "0")
set(GMS_VERSION_MINOR "2")
set(GMS_VERSION_PATCH "biomaps-2")
configure_file(
  "${PROJECT_SOURCE_DIR}/src/gms-config.h.in"
  "${PROJECT_BINARY_DIR}/gms-config.h"
  ESCAPE_QUOTES
  )
include_directories("${PROJECT_BINARY_DIR}")

# Set build type default.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
	"Choose the build type. The options are: None (CMAKE_CXX_FLAGS or
	 CMAKE_C_FLAGS are used), Debug, Release, RelWithDebInfo, MinSizeRel.")
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the build type. The options are: None (CMAKE_CXX_FLAGS or
  CMAKE_C_FLAGS are used), Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()

# Set compiler flags
if (WIN32)
else (WIN32)
	add_definitions("-std=c++11" "-Wall" "-Werror")
	set(PLATFORM_LIBS ${PLATFORM_LIBS} "m")
endif (WIN32)

add_definitions(${LIBXML2_DEFINITIONS})

# Global includes?
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/jsoncpp
  ${SEDML_INCLUDE_DIRS}
  ${LIBCSIM_INCLUDE_DIRS}
  ${LIBSBML_INCLUDE_DIRS}
  ${librdf_INCLUDE_DIRS}
  ${libraptor_INCLUDE_DIRS}
  ${librasqal_INCLUDE_DIRS}
  ${libmicrohttpd_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
)

# Sources
SET(SRCS
  jsoncpp/jsoncpp.cpp
  src/gms.cpp
  src/libhttpd-utils.cpp
  src/gmsApi.cpp
  src/gmsData.cpp
  src/workspace.cpp
  src/contentItem.cpp
  src/xmlDoc.cpp
  src/utils.cpp
  src/rdfgraph.cpp
  src/dojotest.cpp
  src/biomaps.cpp
)

# Headers
SET(HDRS
  src/contentItem.hpp
  src/dojotest.hpp
  src/gmsApi.hpp
  src/gmsData.hpp
  src/libhttpd-utils.hpp
  src/namespaces.hpp
  src/rdfgraph.hpp
  src/utils.hpp
  src/workspace.hpp
  src/xmlDoc.hpp
)

set(EXECUTABLE_NAME "gms")
ADD_EXECUTABLE(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  ${libmicrohttpd_LIBRARIES}
  ${librdf_LIBRARIES}
  ${libraptor_LIBRARIES}
  ${librasqal_LIBRARIES}
  ${LIBCSIM_LIBRARIES}
  ${LIBSBML_LIBRARIES}
  ${SEDML_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${PLATFORM_LIBS}
)
# need this to get things working on Linux?
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    INSTALL_RPATH "\$ORIGIN"
)


INCLUDE(DocBuilder)



# Install the built application and required libraries on the various platforms

set(library_dest_dir bin)
## Need to escape the CMAKE_INSTALL_PREFIX so that it is resolved appropriately when packaging
set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE_NAME}")
IF(APPLE)
  SET(library_dest_dir "${EXECUTABLE_NAME}.app/Contents/MacOS")
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE_NAME}.exe")
ENDIF(WIN32)

# Install the application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
install(TARGETS ${EXECUTABLE_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)

# We need to help resolve the dependencies of LibCSim
MESSAGE(STATUS "CSIM Plugins to install: ${CSIM_PLUGINS}")
INSTALL(FILES ${CSIM_PLUGINS} DESTINATION ${library_dest_dir} COMPONENT Runtime)

# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# plugins are libraries that we want fixed up, but may not be found
# this is the CellML API libraries for libcsim and forcing non-standard system libraries for linux
# as new versions might not always be available
SET(PLUGINS ${CSIM_PLUGINS})

# directories to look for dependencies
function(getSearchPath outDir lib)
    get_filename_component(dir ${lib} PATH)
    message(STATUS "${outDir} dir = ${dir}")
    set(${outDir} ${dir})
    message(STATUS "${outDir} dir = ${dir}")
endfunction(getSearchPath)
list(GET LIBCSIM_LIBRARIES 0 csimLib)
get_filename_component(CSIM_SEARCH_DIR ${csimLib} PATH)
list(GET LIBSBML_LIBRARIES 0 sbmlLib)
get_filename_component(SBML_SEARCH_DIR ${sbmlLib} PATH)
list(GET SEDML_LIBRARIES 0 sedmlLib)
get_filename_component(SEDML_SEARCH_DIR ${sedmlLib} PATH)
list(GET CELLML_LIBRARIES 0 cellmlLib)
get_filename_component(CELLML_SEARCH_DIR ${cellmlLib} PATH)
set(DIRS
    ${CSIM_SEARCH_DIR}
    ${SBML_SEARCH_DIR}
    ${SEDML_SEARCH_DIR}
    ${CELLML_SEARCH_DIR}
)
message(STATUS "DIRS = ${DIRS}")

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
INSTALL(CODE "
    include(BundleUtilities)
    SET(BU_CHMOD_BUNDLE_ITEMS ON)
    fixup_bundle(\"${APPS}\" \"\${PLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)

# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well

# License file for the project, used by the STGZ, NSIS, and PackageMaker generators.
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
install(FILES ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION .)
install(FILES ${CPACK_PACKAGE_DESCRIPTION_FILE} DESTINATION .)

set(CPACK_PACKAGE_VERSION_MAJOR "${GMS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${GMS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${GMS_VERSION_PATCH}")
set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)

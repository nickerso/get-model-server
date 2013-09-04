# General Epithelial Transport - model server
project(get-model-server)

cmake_minimum_required(VERSION 2.8)

# Add in the directory with the FindCellML module
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

IF(WIN32)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ELSE(WIN32)
	#SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF(WIN32)

set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

FIND_PACKAGE(LibMicroHttpd REQUIRED)
FIND_PACKAGE(LibRDF REQUIRED)
FIND_PACKAGE(LibRaptor REQUIRED)
FIND_PACKAGE(LibRasqal REQUIRED)
FIND_PACKAGE(LibCSim REQUIRED)
FIND_PACKAGE(SEDML REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED QUIET)

set(PLATFORM_LIBS "curl" "xml2")

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
  ${CLIBSEDML_INCLUDE_DIRS}
  ${LIBCSIM_INCLUDE_DIRS}
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
)

set(EXECUTABLE_NAME "gms")
ADD_EXECUTABLE(${EXECUTABLE_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  ${libmicrohttpd_LIBRARIES}
  ${librdf_LIBRARIES}
  ${libraptor_LIBRARIES}
  ${librasqal_LIBRARIES}
  ${libcsim_LIBRARIES}
  ${CLIBSEDML_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${PLATFORM_LIBS}
)

INCLUDE(DocBuilder)


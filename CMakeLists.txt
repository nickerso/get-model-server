# General Epithelial Transport - model server
project(get-model-server)

cmake_minimum_required(VERSION 2.8)

# Add in the directory with the FindCellML module
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ELSE(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF(WIN32)

FIND_PACKAGE(LibMicroHttpd REQUIRED)
FIND_PACKAGE(LibCSim REQUIRED)

set(PLATFORM_LIBS "")

# Set compiler flags
if (WIN32)
else(WIN32)
    add_definitions(-Wall -Werror)
    set(PLATFORM_LIBS ${PLATFORM_LIBS} "m")
endif(WIN32)

# Global includes?
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/jsoncpp
  ${libcsim_INCLUDE_DIRS}
  ${libmicrohttpd_INCLUDE_DIRS}
)

# Sources
SET(SRCS
  jsoncpp/jsoncpp.cpp
  src/gms.cpp
  src/libhttpd-utils.cpp
  src/gmsApi.cpp
  src/gmsData.cpp
)

set(EXECUTABLE_NAME "gms")
ADD_EXECUTABLE(${EXECUTABLE_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  ${libmicrohttpd_LIBRARIES}
  ${libcsim_LIBRARIES}
  ${PLATFORM_LIBS}
)

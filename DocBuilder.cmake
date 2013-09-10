
# Find sphinx to generate documentation into the build directory.
find_package(Sphinx)

if (SPHINX_FOUND)
  # List of documenation source files so they show up in Qt-Creator
  set( doc_srcs
	docs/get-ms-index.rst
	docs/get-ms-build.rst
  )

  # Generate html documentation in the build directory.
  add_custom_target(docs ${Sphinx-build_EXECUTABLE} -b html ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}/docs
	SOURCES ${doc_srcs})

endif()

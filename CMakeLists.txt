#
# Minimum version of cmake required
#
cmake_minimum_required(VERSION 3.5.0)
#
#   cmake command line options:
#
#   1) Set CMAKE_TOOLCHAIN_FILE=<path_of_toolchain_file>
#      For Cross-compilation, Set the flag as mentioned.
#
#      example #cmake -DCMAKE_TOOLCHAIN_FILE=<path> ..

## Set default module path if not already set
if(NOT DEFINED CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
endif()
## Include common cmake modules
include(utils)

set(ESMI_OOB "esmi_oob")
set(ESMI_OOB_COMPONENT "lib${ESMI_OOB}")
set(ESMI_OOB_TARGET "${ESMI_OOB}64")
set(SRC_DIR "src/esmi_oob")
set(INC_DIR "include/esmi_oob")

# The following default version values should be updated as appropriate for
# ABI breaks (update MAJOR and MINOR), and ABI/API additions (update MINOR).
# Until ABI stabilizes VERSION_MAJOR will be 0. This should be over-ridden
# by git tags (through "git describe") when they are present.
set(ESMI_OOB_LIB_VERSION_MAJOR 0)
set(ESMI_OOB_LIB_VERSION_MINOR 1)

################# Determine the library version #########################
## Setup the package version based on git tags.
set(LIB_SO_VERSION_STR
              "${ESMI_OOB_LIB_VERSION_MAJOR}.${ESMI_OOB_LIB_VERSION_MINOR}.0")
get_version(${LIB_SO_VERSION_STR} "esmi_oob")

# VERSION_* variables should be set by get_version
set(LIB_SO_VERSION_STR ${VERSION_STRING})
set(${ESMI_OOB}_VERSION_MAJOR "${VERSION_MAJOR}")
set(${ESMI_OOB}_VERSION_MINOR "${VERSION_MINOR}")
set(${ESMI_OOB}_VERSION_PATCH "${VERSION_PATCH}")
set(${ESMI_OOB}_VERSION_BUILD "${VERSION_BUILD}")

# Debian package specific variables
# Set a default value for the package version
set(VERSION_STRING "1.0.0")
get_version(${VERSION_STRING} "e")

# VERSION_* variables should be set by get_version
set(BUILD_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

project(${ESMI_OOB_TARGET})

# Create a configure file to get version info from within library
configure_file(
  "${PROJECT_SOURCE_DIR}/${SRC_DIR}/${ESMI_OOB_TARGET}Config.in"
  "${PROJECT_SOURCE_DIR}/${INC_DIR}/${ESMI_OOB_TARGET}Config.h")

if (NOT DEFINED CPACK_PACKAGE_VENDOR)
    set(CPACK_PACKAGE_VENDOR "AMD")
endif()

if (NOT DEFINED CPACK_PACKAGE_CONTACT)
    set(CPACK_PACKAGE_CONTACT "Advanced Micro Devices Inc.")
endif()

if (NOT DEFINED CPACK_PACKAGE_DESCRIPTION_SUMMARY)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
		"EPYC™ Side Band System Management Interface library")
endif()

set(CPACK_PACKAGE_FILE_NAME "esmi_oob_lib64-${BUILD_VERSION_STRING}")
## Verbose output.
set(CMAKE_VERBOSE_MAKEFILE on)

## Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-rtti -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -std=c++11 ")
# Use this instead of above for 32 bit
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG")
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_common.c")
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_i2c.c")
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_cpuid_msr.c")
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_mailbox.c")
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_rmi.c")
set(ESMI_OOB_SRC_LIST ${ESMI_OOB_SRC_LIST} "${SRC_DIR}/esmi_tsi.c")

set(SMI_EXAMPLE_EXE "esmi_oob_ex")
set(SMI_TOOL "esmi_oob_tool")
set(SMI_CPUID "esmi_oob_cpuid_tool")

add_executable(${SMI_EXAMPLE_EXE} "example/esmi_oob_example.c")
add_executable(${SMI_TOOL} "example/esmi_oob_tool.c")
add_executable(${SMI_CPUID} "example/esmi_oob_cpuid_tool.c")

target_link_libraries(${SMI_EXAMPLE_EXE} ${ESMI_OOB_TARGET})
target_link_libraries(${SMI_TOOL} ${ESMI_OOB_TARGET})
target_link_libraries(${SMI_CPUID} ${ESMI_OOB_TARGET})

add_library(${ESMI_OOB_TARGET} SHARED ${ESMI_OOB_SRC_LIST} ${SMI_INC_LIST})
target_link_libraries(${ESMI_OOB_TARGET} pthread rt m i2c)

## Set the VERSION and SOVERSION values
set_property(TARGET ${ESMI_OOB_TARGET}
                             PROPERTY VERSION "${LIB_SO_VERSION_STR}")
set_property(TARGET ${ESMI_OOB_TARGET}
             PROPERTY SOVERSION "${ESMI_OOB_LIB_VERSION_MAJOR}")

## If the library is a release, strip the target library
if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    add_custom_command(
          TARGET ${ESMI_OOB_TARGET}
          POST_BUILD COMMAND ${CMAKE_STRIP} lib${ESMI_OOB_TARGET}.so)
endif ()

## Define default variable and variables for the optional build target
##  esmi_oob_lib-dev
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	CACHE STRING "Location of ESMI_OOB source code.")
set(CMAKE_INSTALL_PREFIX "/opt/esmi_oob"
                                CACHE STRING "Default installation directory.")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/esmi_oob"
                                      CACHE STRING "Default packaging prefix.")

## Add the install directives for the runtime library.
set(ESMI_OOB_INCLUDE_FILES
    "${INC_DIR}/esmi_common.h"
    "${INC_DIR}/esmi_cpuid_msr.h"
    "${INC_DIR}/esmi_i2c.h"
    "${INC_DIR}/esmi_mailbox.h"
    "${INC_DIR}/esmi_rmi.h"
    "${INC_DIR}/esmi_tsi.h"
   )

set_property(TARGET ${ESMI_OOB_TARGET}
               PROPERTY PUBLIC_HEADER "${ESMI_OOB_INCLUDE_FILES}")

install(TARGETS ${ESMI_OOB_TARGET}
          LIBRARY DESTINATION lib
          PUBLIC_HEADER DESTINATION include/esmi_oob
          COMPONENT ${ESMI_OOB_COMPONENT})

if (ENABLE_EXAMPLES)
  install(TARGETS ${SMI_EXAMPLE_EXE} DESTINATION bin)
  install(TARGETS ${SMI_TOOL} DESTINATION bin)
  install(TARGETS ${SMI_CPUID} DESTINATION bin)
endif()

# Generate Doxygen documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/esmi_oob_doxygen.cfg
                        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.tex
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/docs/esmi_oob_doxygen.cfg
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf
        COMMAND make  > /dev/null
        COMMAND cp  ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf
                ${CMAKE_CURRENT_SOURCE_DIR}/ESMI_OOB_Manual.pdf
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.tex
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex)

  # To create directory "rel" to contain seprate latex
  set(folder "rel")
  add_custom_command(OUTPUT ${folder}
        COMMAND mkdir ${folder}
  )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/release_notes_doxygen.cfg
                        ${CMAKE_CURRENT_BINARY_DIR}/rel/Doxyfile @ONLY)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rel/latex/refman.tex
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/rel/Doxyfile
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/docs/release_notes_doxygen.cfg
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rel)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rel/latex/refman.pdf
        COMMAND make  > /dev/null
        COMMAND cp  ${CMAKE_CURRENT_BINARY_DIR}/rel/latex/refman.pdf
                ${CMAKE_CURRENT_SOURCE_DIR}/ESMI_OOB_Release_Notes.pdf
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/rel/latex/refman.tex
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rel/latex)

  add_custom_target(doc DEPENDS ${folder} ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf
                        ${CMAKE_CURRENT_BINARY_DIR}/rel/latex/refman.pdf)

else()
  message("Doxygen is not found. Will not generate documents.")
endif(DOXYGEN_FOUND)

###############################################################################
#
#    Copyright 2015-2023
#    COSEDA Technologies GmbH
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
###############################################################################

###############################################################################
#  CMakeLists.txe - simple cmake flow
#  Original Author: Paul Ehrlich COSEDA Technologies GmbH
###############################################################################

cmake_minimum_required(VERSION 3.1)

project(SystemCAMS CXX C)

###############################################################################
# Build options
###############################################################################

option (ENABLE_PARALLEL_TRACING "Enable parallel tracing and thus add a pthread dependency." OFF)
option (DISABLE_REFERENCE_NODE_CLUSTERING "Disables clustering for refrence nodes - reference nodes ignored for clustering." OFF)
option (DISABLE_PERFORMANCE_STATISTICS "Disables performance data collection and removes dependency from high precision counter and chrono" OFF)

mark_as_advanced(
        ENABLE_PARALLEL_TRACING
        DISABLE_REFERENCE_NODE_CLUSTERING
        DISABLE_PERFORMANCE_STATISTICS)

add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT ENABLE_PARALLEL_TRACING)
    add_compile_definitions(DISABLE_PARALLEL_TRACING)
endif(NOT ENABLE_PARALLEL_TRACING)  
if(DISABLE_REFERENCE_NODE_CLUSTERING)
    add_compile_definitions(DISABLE_REFERENCE_NODE_CLUSTERING)
endif(DISABLE_REFERENCE_NODE_CLUSTERING)    
if(DISABLE_PERFORMANCE_STATISTICS)
    add_compile_definitions(DISABLE_PERFORMANCE_STATISTICS)
endif(DISABLE_PERFORMANCE_STATISTICS)   


###############################################################################
# Version
###############################################################################

set(SystemCAMS_VERSION_FILE "${PROJECT_SOURCE_DIR}/src/config.h")
if(EXISTS ${SystemCAMS_VERSION_FILE})
    file(READ ${SystemCAMS_VERSION_FILE} SYSTEMCAMS_VERSION_FILE_CONTENT)
     # TODO improve regex
     string(REGEX MATCH "#define PACKAGE_VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" REGEX_VERSION_MATCH ${SYSTEMCAMS_VERSION_FILE_CONTENT})
     set(SystemCAMS_VERSION ${CMAKE_MATCH_1})
    if(NOT SystemCAMS_VERSION)
        message(FATAL_ERROR "Unable to parse the version file ${SystemCAMS_VERSION_FILE}")
    endif()
else(EXISTS ${SystemCAMS_VERSION_FILE})
    message (FATAL_ERROR "Unable to read SystemCAMS version file")
endif(EXISTS ${SystemCAMS_VERSION_FILE})

###############################################################################
# Configure status
###############################################################################

if (ENABLE_PARALLEL_TRACING)
  message ("ENABLE_PARALLEL_TRACING = ${ENABLE_PARALLEL_TRACING}")
else (ENABLE_PARALLEL_TRACING)
  message (STATUS "ENABLE_PARALLEL_TRACING = ${ENABLE_PARALLEL_TRACING}")
endif (ENABLE_PARALLEL_TRACING)

if (DISABLE_REFERENCE_NODE_CLUSTERING)
  message ("DISABLE_REFERENCE_NODE_CLUSTERING = ${DISABLE_REFERENCE_NODE_CLUSTERING}")
else (DISABLE_REFERENCE_NODE_CLUSTERING)
  message (STATUS "DISABLE_REFERENCE_NODE_CLUSTERING = ${DISABLE_REFERENCE_NODE_CLUSTERING}")
endif (DISABLE_REFERENCE_NODE_CLUSTERING)

if (DISABLE_PERFORMANCE_STATISTICS)
  message ("DISABLE_PERFORMANCE_STATISTICS = ${DISABLE_PERFORMANCE_STATISTICS}")
else (DISABLE_PERFORMANCE_STATISTICS)
  message (STATUS "DISABLE_PERFORMANCE_STATISTICS = ${DISABLE_PERFORMANCE_STATISTICS}")
endif (DISABLE_PERFORMANCE_STATISTICS)

if(SYSTEMC_HOME)
    include_directories(${SYSTEMC_HOME}/include)
elseif(ENV{SYSTEMC_HOME})
    include_directories($ENV{SYSTEMC_HOME}/include)
else()
    message(FATAL_ERROR "SYSTEMC_HOME Environment or CMake variable is not defined")
endif()

macro(install_headers)
	string(REPLACE ${CMAKE_SOURCE_DIR}/src "" REL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	install(FILES ${ARGV} DESTINATION include/${REL_DIR})
endmacro(install_headers)

add_subdirectory(src)

install(
	FILES 
		AUTHORS
		ChangeLog
		COPYING
		INSTALL
		LICENSE
		NEWS
		NOTICE
		README
		RELEASENOTES
	DESTINATION .)

###############################################################################
# Set the installation paths
###############################################################################

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (IS_ABSOLUTE $ENV{SYSTEMCAMS_HOME})
        set (_CMAKE_INSTALL_PREFIX $ENV{SYSTEMCAMS_HOME})
  elseif (UNIX)
    set (_CMAKE_INSTALL_PREFIX "/opt/systemc-ams")
  elseif (WIN32)
    file (TO_CMAKE_PATH $ENV{ProgramFiles} _WIN32_INSTALL_PREFIX)
    set (_CMAKE_INSTALL_PREFIX "${_WIN32_INSTALL_PREFIX}/SystemC-AMS")
    unset (_WIN32_INSTALL_PREFIX)
  else (EXISTS $ENV{SYSTEMC_ROOT_DIR})
    set (_CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/systemc-ams")
  endif (IS_ABSOLUTE $ENV{SYSTEMC_HOME})
  set (CMAKE_INSTALL_PREFIX ${_CMAKE_INSTALL_PREFIX} CACHE PATH "Installation root directory" FORCE)
  unset (_CMAKE_INSTALL_PREFIX)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Set the installation paths for headers, libraries, and documentation.
set (CMAKE_INSTALL_DOCDIR "share/doc/systemc-ams" CACHE PATH
     "Documentation installation directory") # otherwise mixed-case
if (INSTALL_TO_LIB_BUILD_TYPE_DIR)
  # Install libraries to lib-${CMAKE_BUILD_TYPE} so that different build variants
  # of the library can be installed in parallel to the same SystemC root
  # directory. The find_package() mechanism in config-mode of CMake will then
  # automatically find the correct libraries matching the chosen
  # ${CMAKE_BUILD_TYPE}.
  set (_INSTALL_LIBDIR lib-${CMAKE_BUILD_TYPE})
  string (TOLOWER ${_INSTALL_LIBDIR} _INSTALL_LIBDIR)
  set (CMAKE_INSTALL_LIBDIR ${_INSTALL_LIBDIR} CACHE PATH
       "Object code libraries installation directory" FORCE)
  unset (_INSTALL_LIBDIR)
elseif (INSTALL_TO_LIB_TARGET_ARCH_DIR)
  # Some build systems assume to find SystemC in lib-<target-arch>
  set (CMAKE_INSTALL_LIBDIR lib-${SystemCAMS_TARGET_ARCH} CACHE PATH
       "Object code libraries installation directory" FORCE)
endif (INSTALL_TO_LIB_BUILD_TYPE_DIR)
include (GNUInstallDirs)

# Install symlink lib-<target-arch> to lib-${CMAKE_BUILD_TYPE}
if (INSTALL_LIB_TARGET_ARCH_SYMLINK AND CMAKE_HOST_UNIX)
  install (CODE "
  EXECUTE_PROCESS(COMMAND cmake -E create_symlink ${CMAKE_INSTALL_LIBDIR} lib-${SystemCAMS_TARGET_ARCH}
                    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})
  ")
endif (INSTALL_LIB_TARGET_ARCH_SYMLINK AND CMAKE_HOST_UNIX)

if (INSTALL_TO_LIB_BUILD_TYPE_DIR OR INSTALL_TO_LIB_TARGET_ARCH_DIR)
    set (SystemCAMS_INSTALL_CMAKEDIR share/cmake/SystemCAMS CACHE PATH
       "CMake package configuration installation directory" FORCE)
else (INSTALL_TO_LIB_BUILD_TYPE_DIR OR INSTALL_TO_LIB_TARGET_ARCH_DIR)
    set (SystemCAMS_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/SystemCAMS CACHE PATH
        "CMake package configuration installation directory for the SystemCAMS package.")
endif (INSTALL_TO_LIB_BUILD_TYPE_DIR OR INSTALL_TO_LIB_TARGET_ARCH_DIR)
mark_as_advanced(SystemCAMS_INSTALL_CMAKEDIR)
set(SystemCAMS_INSTALL_FULL_CMAKEDIR "${CMAKE_INSTALL_PREFIX}/${SystemCAMS_INSTALL_CMAKEDIR}")
	
###############################################################################
# Provide package of the SystemCAMS library to other programs using CMake
###############################################################################

# Export the systemc-ams library target for usage by other programs
# ... to local build tree
export (TARGETS systemc-ams
        NAMESPACE SystemC::
        FILE SystemCAMSTargets.cmake)
# ... to installation tree
install (EXPORT SystemCAMSTargets
         NAMESPACE SystemC::
         DESTINATION ${SystemCAMS_INSTALL_CMAKEDIR}
         COMPONENT dev
         )

# Register the SystemCAMS package for use from the
# build tree. (This registers the build tree with the global CMake-registry.)
export (PACKAGE SystemCAMS)

# Create the <Package>Config.cmake and <Package>ConfigVersion files
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/SystemCAMSConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SystemCAMSConfig.cmake
    INSTALL_DESTINATION ${SystemCAMS_INSTALL_CMAKEDIR})
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SystemCAMSConfigVersion.cmake"
    VERSION ${SystemCAMS_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install the <Package>Config.cmake and <Package>ConfigVersion.cmake
install(FILES "${PROJECT_BINARY_DIR}/SystemCAMSConfig.cmake"
        "${PROJECT_BINARY_DIR}/SystemCAMSConfigVersion.cmake"
        DESTINATION "${SystemCAMS_INSTALL_CMAKEDIR}"
        COMPONENT dev)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(O2_Balancer2)

# Check if you're in the root directory of the project, and give an error.
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Please create a dedicated build directory for this load balancer. (You may need remove the CmakeCache.txt and the cache)")
ENDIF()

# Add custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(macros)

# Enable c++11 even when cmake < 3.1
use_cxx11()

# Enable pedantic errors and warnings for c++
use_cxx_warning_pedantic()

# Required packages 
find_package(FairMQ REQUIRED)
find_package(Zookeeper REQUIRED)

# Include Commonly required headers
include_directories(${FairMQ_INCDIR}) # INCDIR Triggered
include_directories(${ZOOKEEPER_INCLUDE_DIR})

# Set Commonly required libraries
set(O2_Balancer2_LIBRARIES
	${FairMQ_LIBDIR} # LIBDIR Triggered
)

# Commonly shared library
add_subdirectory(lib)

# Information Control node binary
add_subdirectory(icn)

# Event Processing node binary
add_subdirectory(epn)

# First Line Processing binary
add_subdirectory(flp)

# Unit tests to validate application
IF(ENABLE_TESTS)
	ENABLE_TESTING()
	add_subdirectory(tests)
ENDIF()
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

# Configurable options and defaults
OPTION(ENABLE_TESTS "Enable unit tests" ON)
OPTION(ENABLE_CODECOV "Measure code coverage" OFF)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(FairMQ REQUIRED)
find_package(FairLogger REQUIRED)
find_package(Zookeeper REQUIRED)
find_package(
    Boost 1.59 COMPONENTS
    system
    timer
    program_options
    filesystem
    chrono
    exception
    serialization
    log
    log_setup
    unit_test_framework
    date_time
    REQUIRED
)

# Include Commonly required headers
include_directories(${ZeroMQ_INCLUDE_DIR})
include_directories(${FairMQ_INCDIR}/fairmq) # INCDIR Triggered
include_directories(${FairLogger_INCLUDE_DIR})
include_directories(${ZOOKEEPER_INCLUDE_DIR})

# Set Commonly required libraries
set(O2_Balancer2_LIBRARIES
	Threads::Threads
	Boost::boost
	Boost::system
	Boost::program_options
  	Boost::thread
	Boost::log
	Boost::log_setup
	${ZeroMQ_LIBRARY}
	${ZOOKEEPER_LIBRARIES}
	FairLogger
	FairMQ
)

if(ENABLE_CODECOV)
	message("O2B2: Running CodeCoverage, forcing ENABLE_TESTS & CMAKE_BUILD_TYPE=Debug")
	include(CodeCoverage)
	setup_target_for_coverage(coverage testexample coverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
	set(ENABLE_TESTS ON) # tests need to be enabled for code coverage
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		message(WARNING "Forcing buld type to Debug to allow code coverage!")
		set(CMAKE_BUILD_TYPE Debug) # debug needed to disable compilter optimization
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("O2B2: Disabling gcc optimization & adding extensive debug symbols")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	message("O2B2: Optimization the living shit out of gcc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

# Commonly shared library
add_subdirectory(lib)

# Information Control node binary
add_subdirectory(icn)

# Event Processing node binary
add_subdirectory(epn)

# First Line Processing binary
add_subdirectory(flp)

# Unit tests to validate application
if(ENABLE_TESTS)
	message("O2B2: Enabling tests")
	ENABLE_TESTING()
	add_subdirectory(tests)
endif()

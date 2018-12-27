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
OPTION(ENABLE_DOXY "Generate documentation" ON)
OPTION(ENALBE_TRAVIS "If this build is being run on travis.ci" OFF)

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
    program_options
    filesystem
    chrono
    exception
    serialization
    log
    log_setup
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
	if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(WARNING "Forcing buld type to Debug to allow code coverage!")
		set(CMAKE_BUILD_TYPE Debug) # debug needed to disable compilter optimization
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("O2B2: Disabling gcc optimization & adding extensive debug symbols")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	message("O2B2: Optimizing the living shit out of gcc")
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

# Generate Doxygen Documentation
if(ENABLE_DOXY)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
	    # set input and output files
	    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cnf)
	    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf)

	    # request to configure the file
	    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	    message("O2B2: Doxygen build started")

	    # note the option ALL which allows to build the docs together with the application
	    if(NOT ENABLE_TRAVIS)
	    	add_custom_target(doc_doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM )
	    endif(NOT ENABLE_TRAVIS)
	else (DOXYGEN_FOUND)
	  message("O2B2: Doxygen needs to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif()

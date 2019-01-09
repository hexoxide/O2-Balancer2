project(tests)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/icn/include)
include_directories(${CMAKE_SOURCE_DIR}/flp/include)
include_directories(${CMAKE_SOURCE_DIR}/epn/include)

set(LIB_INTEGRATION_SRC
	${CMAKE_SOURCE_DIR}/icn/src/icn.cxx
	${CMAKE_SOURCE_DIR}/flp/src/flp.cxx
	${CMAKE_SOURCE_DIR}/epn/src/epn.cxx
)

set(LIB_INTEGRATION_HEADERS
	${CMAKE_SOURCE_DIR}/icn/include/icn.h
	${CMAKE_SOURCE_DIR}/flp/include/flp.h
	${CMAKE_SOURCE_DIR}/epn/include/epn.h
)

add_library(O2Integration SHARED ${LIB_INTEGRATION_SRC} ${LIB_INTEGRATION_HEADERS})
set_target_properties(O2Integration PROPERTIES LINKER_LANGUAGE CXX)

find_package(
    Boost 1.53 COMPONENTS
    timer
    unit_test_framework
    REQUIRED
)

set(O2_Balancer2_TEST_LIBRARIES
	Boost::timer
	Boost::unit_test_framework
)

set(TEST_EXAMPLE_SOURCE 
	src/test-example.cxx
)

set(TEST_EXCEPTION_SOURCE 
	src/test-exception.cxx
)

set(TEST_O2NETWORK_SOURCE 
	src/test-network.cxx
)

set(TEST_O2IP_SOURCE 
	src/test-o2ip.cxx
)

set(TEST_O2CHANNEL_SOURCE 
	src/test-o2channel.cxx
)

set(TEST_EPN_SOURCE 
	src/test-epn.cxx
)

message("O2B2: test example cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testexample ${TEST_EXAMPLE_SOURCE})

message("O2B2: test exception cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testexception ${TEST_EXCEPTION_SOURCE})

message("O2B2: test o2network cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testo2network ${TEST_O2NETWORK_SOURCE})

message("O2B2: test o2ip cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testo2ip ${TEST_O2IP_SOURCE})

message("O2B2: test o2channel cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testo2channel ${TEST_O2CHANNEL_SOURCE})

message("O2B2: test epn cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testepn ${TEST_EPN_SOURCE})

target_link_libraries(
	testexample
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testexception
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2network
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2ip
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2channel
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testepn
	O2
	O2Integration
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

add_test(TestExample testexample)
add_test(TestException testexception)
add_test(TestO2Network testo2network)
add_test(TestO2Ip testo2ip)
add_test(TestO2Channel testo2channel)
add_test(TestEPN testepn)
project(tests)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

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

target_link_libraries(
	testexample
	O2
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testexception
	O2
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2network
	O2
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2ip
	O2
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

target_link_libraries(
	testo2channel
	O2
	${O2_Balancer2_LIBRARIES}
	${O2_Balancer2_TEST_LIBRARIES}
)

add_test(TestExample testexample)
add_test(TestException testexception)
add_test(TestO2Network testo2network)
add_test(TestO2Ip testo2ip)
add_test(TestO2Channel testo2channel)

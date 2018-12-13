project(tests)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

find_package(
    Boost 1.53 COMPONENTS
    unit_test_framework
    REQUIRED
)

set(TEST_EXAMPLE_SOURCE 
	src/test-example.cxx
)

message("O2B2: testexample cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(testexample ${TEST_EXAMPLE_SOURCE})

target_link_libraries(
	testexample
	O2
	${O2_Balancer2_LIBRARIES}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(TestExample testexample)
project(epn)

set(BIN_EPN_SOURCE
	src/main.cxx
)

set(BIN_EPN_HEADERS
	#include/
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

add_executable(epn ${BIN_EPN_SOURCE})

target_link_libraries(epn libo2 ${O2_Balancer2_LIBRARIES})
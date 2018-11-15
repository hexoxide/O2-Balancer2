project(icn)

set(BIN_ICN_SOURCE
	src/main.cxx
)

set(BIN_ICN_HEADERS
	#include/
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

add_executable(icn ${BIN_ICN_SOURCE})

target_link_libraries(icn libo2 ${O2_Balancer2_LIBRARIES})
project(flp)

set(BIN_FLP_SOURCE
	src/main.cxx
)

set(BIN_FLP_HEADERS
	#include/
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

add_executable(flp ${BIN_FLP_SOURCE})

target_link_libraries(flp libo2 ${O2_Balancer2_LIBRARIES})
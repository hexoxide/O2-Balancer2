project(flp)

set(BIN_FLP_SOURCE
	src/main.cxx
	src/flp.cxx
)

set(BIN_FLP_HEADERS
	include/flp.h
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(include)

message("O2B2: FLP cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(flp ${BIN_FLP_SOURCE} ${BIN_ICN_HEADERS})

target_link_libraries(flp libo2 ${O2_Balancer2_LIBRARIES})
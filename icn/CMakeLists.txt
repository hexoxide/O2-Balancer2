project(icn)

set(BIN_ICN_SOURCE
	src/main.cxx
	src/icn.cxx
)

set(BIN_ICN_HEADERS
	include/icn.h
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(include)

message("O2B2: ICN cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(icn ${BIN_ICN_SOURCE} ${BIN_ICN_HEADERS})

target_link_libraries(icn libo2 ${O2_Balancer2_LIBRARIES})
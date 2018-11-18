project(epn)

set(BIN_EPN_SOURCE
	src/main.cxx
	src/epn.cxx
)

set(BIN_EPN_HEADERS
	include/epn.h
)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(include)

message("O2B2: EPN cxx flags: ${CMAKE_CXX_FLAGS}")
add_executable(epn ${BIN_EPN_SOURCE} ${BIN_EPN_HEADERS})

target_link_libraries(epn libo2 ${O2_Balancer2_LIBRARIES})
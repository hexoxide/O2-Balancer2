project(O2)

set(LIB_O2_SRC 
	src/o2channel.cxx
	src/o2data.cxx
)

set(LIB_O2_HEADERS
	include/o2channel.h 
	include/o2data.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include) 
add_library(O2 SHARED ${LIB_O2_SRC} ${LIB_O2_HEADERS})
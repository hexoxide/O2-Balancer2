project(O2)

set(LIB_O2_SRC
	src/clone.cxx
	src/data/o2channel.cxx 
	src/data/o2ip.cxx 
	src/linux-interface.cxx
)

set(LIB_O2_HEADERS.
	include/clone.h
	include/data/o2channel.h 
	include/data/o2data.h
	include/data/o2ip.h
	include/linux-interface.h
	include/o2exception.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/data)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include) 
add_library(O2 SHARED ${LIB_O2_SRC} ${LIB_O2_HEADERS})
set_target_properties(O2 PROPERTIES LINKER_LANGUAGE CXX)
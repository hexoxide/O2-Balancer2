project(O2Lib)

set(LIB_O2LIB_SRC 
	src/libo2.cxx
)

set(LIB_O2LIB_HEADERS 
	include/libo2.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include) 
add_library(libo2 SHARED ${LIB_O2LIB_SRC} ${LIB_O2LIB_HEADERS})
cmake_minimum_required(VERSION 3.15)
project(Coroutine)
set(CMAKE_CXX_STANDARD 20)
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines -pthread" )
list ( APPEND PRJ_COMPILE_FEATURES cxx_std_20 )
add_library(rcoutine scheduler.hpp)
target_compile_features ( rcoutine
	PRIVATE 
		${PRJ_COMPILE_FEATURES}
)

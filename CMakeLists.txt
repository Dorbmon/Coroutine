cmake_minimum_required(VERSION 3.15)
project(Coroutine)
set(CMAKE_CXX_STANDARD 20)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
list ( APPEND PRJ_COMPILE_FEATURES cxx_std_20 )
add_executable(rcoroutine scheduler.cpp promise.cpp thread.cpp worker.cpp core.cpp callback.h main.cpp)
target_compile_features ( rcoroutine
	PRIVATE 
		${PRJ_COMPILE_FEATURES}
)

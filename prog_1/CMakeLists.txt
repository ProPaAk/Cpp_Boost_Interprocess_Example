cmake_minimum_required(VERSION 3.20)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

project (${ProjectId})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lrt")
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -pthread -Wl,--no-whole-archive")

set (BOOST_ROOT)
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)
set (CMAKE_VERBOSE_MAKEFILE 1)
set (CMAKE_THREAD_LIBS_INIT)

find_package(Boost 1.71.0 REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(${ProjectId} 
	main.cpp	
)

target_link_libraries(${ProjectId} ${Boost_LIBRARIES} rt)
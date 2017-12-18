cmake_minimum_required (VERSION 3.0.0)

#Database Access Proxy
set (PROJECT daproxy)
project(${PROJECT})

set ("${PROJECT}_VERSION_MAJ" 1)
set ("${PROJECT}_VERSION_MIN" 0)
set ("${PROJECT}_VERSION_A" 0)
set ("${PROJECT}_VERSION_B" 0)
set ("${PROJECT}_VERSION" ${${PROJECT}_VERSION_MAJ}0${${PROJECT}_VERSION_MIN}0${${PROJECT}_VERSION_A}0${${PROJECT}_VERSION_B})

message(STATUS ${${PROJECT}_VERSION})

#Set prebuilt boost libraries here:
set (BOOST_ROOT /home/alec/boost_1_58_0/stage)
add_definitions("-DBOOST_ROOT=/home/alec/boost_1_58_0/stage")

#Use ${BOOST_ROOT} var:
set (BOOST_INCLUDE_DIR ${BOOST_ROOT}/include)
set (Boost_LIBRARIES ${BOOST_ROOT}/lib)
set (BOOST_LIBS ${BOOST_ROOT}/lib)
#Build type:
if(${CMAKE_BUILD_TYPE}  STREQUAL "Debug")
    set(Boost_USE_STATIC_LIBS        OFF)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -std=gnu++11 -static-libgcc -static-libstdc++ -lpthread -L${Boost_LIBRARIES} -lboost_regex -m64 -fpermissive -g ")
elseif(${CMAKE_BUILD_TYPE}  STREQUAL "Release")
    set(Boost_USE_STATIC_LIBS        ON)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -std=gnu++11 -static-libgcc -static-libstdc++ -lpthread -L${Boost_LIBRARIES} -lboost_regex -m64 -fpermissive -O2 -s ")
else()
    #Slow debug build:
    set(Boost_USE_STATIC_LIBS        ON)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -std=gnu++11 -static-libgcc -static-libstdc++ -lpthread -L${Boost_LIBRARIES} -lboost_regex -m64 -fpermissive -g ")
endif()

#boost libraries:
find_package(Threads REQUIRED)
find_package(Boost 1.58.0 COMPONENTS regex system thread coroutine context filesystem date_time REQUIRED)
set(Boost_USE_MULTITHREADED ON)
set (LIBXML2_INCLUDE /usr/include/libxml2)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version >=4.8 required.")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Clang version >=3.3 required.")
    endif()
elseif (MSVC14)
else()
    message(WARNING "Your compiler (${CMAKE_CXX_COMPILER_ID}) has not been tested on this project. Only Clang and GCC has been tested. Please report any problems at the project page on GitHub.")
endif()

include_directories(${SYS_INCLUDE})
#include_directories(${MYSQL_INCLUDE})
#include_directories(${MYSQL_INCLUDE_CONN})
include_directories(SYSTEM ${BOOST_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/inc)

file(GLOB_RECURSE SRCS src/*.c*)
file(GLOB_RECURSE HDRS inc/*.h)

message("Boost libraries: ${Boost_LIBRARIES}")

add_executable(${PROJECT} ${HDRS} ${SRCS} main.cpp)

target_link_libraries(${PROJECT} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT} ${CMAKE_DL_LIBS})

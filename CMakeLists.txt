cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/*.hpp
)

project(http-ser LANGUAGES CXX)

set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_MULTITHREADED ON)
add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )

find_package(Boost REQUIRED COMPONENTS
    system
    thread
    exception
)

find_package (Threads)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    include_directories(${BOOST_INCLUDE_DIRS})
    message("Boost_INCLUDE_DIR = " ${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIRS})
    add_definitions("-DHAS_BOOST")

    add_definitions("-DBOOST_THREAD_USE_LIB")

    include_directories(${BOOST_ROOT})
endif()

add_executable(http-ser ${SOURCES})

target_link_libraries (http-ser ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(http-ser PUBLIC "${PROJECT_BINARY_DIR}")
target_compile_features(http-ser PRIVATE cxx_std_17)

target_compile_options(http-ser PRIVATE
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    -fno-sanitize=vptr
    -fno-omit-frame-pointer
)

target_link_options(http-ser PRIVATE
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    -fno-sanitize=vptr
    -fno-omit-frame-pointer
)

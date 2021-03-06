cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/*.hpp
)

project(http-ser LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_MULTITHREADED ON)
add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )

find_package(Boost REQUIRED COMPONENTS
    system
    filesystem
    thread
    exception
)
#find_package(PkgConfig REQUIRED)

find_package (Threads)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
	-fsanitize=address
	-fsanitize=leak
	-fsanitize=undefined
	-fno-sanitize=vptr
	-fno-omit-frame-pointer
)

target_link_options(${PROJECT_NAME} PRIVATE
	-fsanitize=address
	-fsanitize=leak
	-fsanitize=undefined
	-fno-sanitize=vptr
	-fno-omit-frame-pointer
)

add_definitions("-DHAS_BOOST")
add_definitions("-DBOOST_THREAD_USE_LIB")

#if (UNIX)
	#pkg_check_modules (GIO2 gio-2.0)
	#target_include_directories(${PROJECT_NAME} PUBLIC ${GIO2_INCLUDE_DIRS})
	#target_link_libraries(${PROJECT_NAME} ${GIO2_LIBRARIES})
#endif()

include_directories(${Boost_INCLUDE_DIR})
include_directories(${BOOST_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${BOOST_ROOT})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

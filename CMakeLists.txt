cmake_minimum_required(VERSION 3.15)
project(test_project)
set(CMAKE_CXX_STANDARD 11)

# Add source files
set(SOURCE_FILES main.cpp glad.c)

# Boost
#set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")
#SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/boost/boost")
#SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/boost/libs")
#find_package(Boost REQUIRED thread)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Add executable target
add_executable(test_project ${SOURCE_FILES})

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Include headers
target_include_directories(test_project PUBLIC "include")
target_include_directories(test_project PUBLIC "craft")

# Build GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw)
target_link_libraries(test_project glfw)

# Boost
#TARGET_LINK_LIBRARIES( test_project LINK_PUBLIC ${Boost_LIBRARIES} )

# Copy assets from source to build directory
FILE(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
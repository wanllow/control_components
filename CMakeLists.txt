cmake_minimum_required(VERSION 3.5)

project(control_components)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/third_party_x86.cmake)

file(GLOB_RECURSE HDRS "include/*.h")

file(GLOB_RECURSE SRCS "src/*.cpp")

file(GLOB_RECURSE TESTC "test/*.cpp")
file(GLOB_RECURSE TESTH "include/*.h")

add_executable(main ${SRCS} ${TESTC} ${HDRS} ${TESTH})
target_compile_features(main PUBLIC cxx_std_11)
target_include_directories(main PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${THIRD_PARTY_INCLUDE}
)
target_link_libraries(main PUBLIC  ${THIRD_PARTY_LIB})
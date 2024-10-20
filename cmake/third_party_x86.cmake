# 第三方库路径
set(THIRD_PARTY_PATH "${PROJECT_SOURCE_DIR}/third_party")

# # asio库
# add_library(asio INTERFACE)
# set(ASIO_PATH ${THIRD_PARTY_PATH}/asio-1.14.0)
# set(ASIO_INCLUDE ${ASIO_PATH}/include)
# target_include_directories(asio INTERFACE ASIO_INCLUDE)

# # boost库
# add_library(boost INTERFACE)
# set(BOOST_PATH ${THIRD_PARTY_PATH}/boost_1_78_0)
# set(BOOST_INCLUDE ${BOOST_PATH}/include)
# set(BOOST_LIB ${BOOST_PATH}/lib)
# file(GLOB BOOST_LIBS ${BOOST_LIB}/libboost*.so)
# link_directories(${BOOST_LIB})
# target_link_libraries(boost INTERFACE ${BOOST_LIBS})

# # eigen库
# add_library(eigen INTERFACE)
# target_compile_definitions(eigen INTERFACE EIGEN_STACK_ALLOCATION_LIMIT=4194304)
# set(EIGEN_PATH ${THIRD_PARTY_PATH}/eigen-3.3.7)
# target_include_directories(eigen INTERFACE ${EIGEN_PATH}/include/eigen3)

# # protobuf库
# add_library(protobuf INTERFACE)
# set(PROTOBUF_PATH ${THIRD_PARTY_PATH}/protobuf-3.3.0)
# target_include_directories(protobuf INTERFACE ${PROTOBUF_PATH}/include)
# link_directories(${PROTOBUF_PATH}/lib)
# target_link_libraries(protobuf INTERFACE libprotobuf.a)

# # yaml-cpp库
# add_library(yaml-cpp INTERFACE)
# set(YAML_CPP_PATH ${THIRD_PARTY_PATH}/yaml-cpp-0.6.2)
# set(YAML_CPP_INCLUDE ${YAML_CPP_PATH}/include)
# target_include_directories(yaml-cpp INTERFACE ${YAML_CPP_PATH}/include)
# link_directories(${YAML_CPP_PATH}/lib)
# target_link_libraries(yaml-cpp INTERFACE libyaml-cpp.so.0.6.2)

# # zlib库
# add_library(zlib INTERFACE)
# set(ZLIB_PATH ${THIRD_PARTY_PATH}/zlib-1.2.11)
# target_include_directories(zlib INTERFACE ${ZLIB_PATH}/include)
# link_directories(${ZLIB_PATH}/lib)
# target_link_libraries(zlib INTERFACE libz.so)


# # g3log库
# add_library(g3log-1.3.4 INTERFACE)
# set(G3LOG_PATH ${THIRD_PARTY_PATH}/g3log-1.3.4)
# set(G3LOG_INCLUDE ${G3LOG_PATH}/include)
# target_include_directories(g3log-1.3.4 INTERFACE ${G3LOG_PATH}/include)
# link_directories(${G3LOG_PATH}/lib)
# target_link_libraries(g3log-1.3.4 INTERFACE libg3log.a)

# # g3sinks库
# add_library(g3log INTERFACE)
# set(G3SINKS_PATH ${THIRD_PARTY_PATH}/g3sinks)
# set(G3SINK2_INCLUDE ${G3SINKS_PATH}/include)
# target_include_directories(g3log INTERFACE ${G3SINKS_PATH}/include)
# link_directories(${G3SINKS_PATH}/lib)
# file(GLOB G3SINKS_LIBS ${G3SINKS_PATH}/lib/*.a)
# target_link_libraries(g3log INTERFACE ${G3SINKS_LIBS} zlib boost g3log-1.3.4)

# # lisa库
# add_library(lisa INTERFACE)
# set(LISA_PATH ${THIRD_PARTY_PATH}/lisa)
# set(LISA_INCLUDE ${LISA_PATH}/include)
# target_include_directories(lisa INTERFACE ${LISA_PATH}/include)
# link_directories(${LISA_PATH}/lib)
# file(GLOB LISA_LIBS ${LISA_PATH}/lib/*.a)
# file(GLOB LISA_SO_LIBS ${LISA_PATH}/lib/*.so)
# target_link_libraries(lisa INTERFACE ${LISA_LIBS} ${LISA_SO_LIBS})

# # curl库
# add_library(curl INTERFACE)
# set(CURL_PATH ${THIRD_PARTY_PATH}/curl-4.6.0)
# target_include_directories(curl INTERFACE ${CURL_PATH}/include)
# link_directories(${CURL_PATH}/lib)
# file(GLOB CURL_LIBS ${CURL_PATH}/lib/lib*)
# target_link_libraries(curl INTERFACE ${CURL_LIBS})


# # absl库
# add_library(absl INTERFACE)
# set(ABSL_PATH ${THIRD_PARTY_PATH}/abseil-cpp)
# target_include_directories(absl INTERFACE ${ABSL_PATH}/include)
# link_directories(${ABSL_PATH}/lib)
# file(GLOB absl_LIBS ${ABSL_PATH}/lib/*.so)
# target_link_libraries(absl INTERFACE ${absl_LIBS})

# # opencv
# add_library(opencv INTERFACE)
# set(OPENCV_PATH ${THIRD_PARTY_PATH}/opencv-3.4.0)
# set(OPENCV_INCLUDE ${OPENCV_PATH}/include)
# set(OPENCV_LIB ${OPENCV_PATH}/lib)
# target_include_directories(opencv INTERFACE ${OPENCV_INCLUDE})
# link_directories(${OPENCV_LIB})
# file(GLOB OPENCV_LIBS ${OPENCV_PATH}/lib/*.a)
# target_link_libraries(opencv INTERFACE libopencv_world.so )

# # tinyfsm
# set(TINYFSM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyfsm)
# set(TINYFSM_INCLUDE ${TINYFSM_PATH}/include)

# # cxxopt
# set(CXXOPT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cxxopts)
# set(CXXOPT_INCLUDE ${CXXOPT_PATH}/include)

# eigen库
add_library(eigen INTERFACE)
target_compile_definitions(eigen INTERFACE EIGEN_STACK_ALLOCATION_LIMIT=4194304)
set(EIGEN_PATH ${THIRD_PARTY_PATH}/eigen-3.4.0)
set(EIGEN_INCLUDE ${EIGEN_PATH})
target_include_directories(eigen INTERFACE ${EIGEN_PATH}/include/eigen3)

# all 
# Set(THIRD_PARTY_LIB asio pthread lisa g3log yaml-cpp boost opencv)
Set(THIRD_PARTY_LIB eigen)
set(THIRD_PARTY_INCLUDE "")
# 
foreach (include_var EIGEN_INCLUDE)
    list(APPEND THIRD_PARTY_INCLUDE "${${include_var}}")
endforeach()
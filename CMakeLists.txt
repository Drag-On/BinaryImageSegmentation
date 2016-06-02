cmake_minimum_required(VERSION 3.5)
project(Sheet_6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(OpenCV 3.0 CONFIG REQUIRED HINTS /work/opencv30/)
if (OpenCV_FOUND)
    message(STATUS "Found OpenCV v${OpenCV_VERSION}${OpenCV_VERSION_STATUS}")
    message("opencv include directories: ${OpenCV_INCLUDE_DIRS}")
    message("opencv libs: ${OpenCV_LIBS}")
endif ()

find_package(Eigen3 REQUIRED)
if(Eigen3_FOUND)
    message("Eigen3 found!")
endif()

set(SOURCE_FILES main.cpp MoGDistribution.hh MoGDistribution.h StringHelper.h GraphCut/BinaryGraphCutOptimizer.cpp GraphCut/BinaryGraphCutOptimizer.h GraphCut/EdgeGraph.cpp GraphCut/EdgeGraph.h)
set(MAXFLOW_FILES maxflow-v3.04.src/block.h maxflow-v3.04.src/graph.cpp maxflow-v3.04.src/graph.h maxflow-v3.04.src/maxflow.cpp)
add_executable(Sheet_6 ${SOURCE_FILES} ${MAXFLOW_FILES})
target_include_directories(Sheet_6 PUBLIC maxflow-v3.04.src ${OpenCV_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
target_link_libraries(Sheet_6 ${OpenCV_LIBS})

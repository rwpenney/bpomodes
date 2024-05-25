# CMake control-file for small-sample entropy estimator
# RW Penney, September 2023

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(bpo-modes)
ENABLE_TESTING()

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(GNUCC_OPTS -ansi -pedantic -Wall)
    SET(CMAKE_C_FLAGS_DEBUG:STRING "-ggdb")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

FIND_PACKAGE(Boost 1.60 REQUIRED COMPONENTS program_options)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

SET(lib_hdrs
    bpomodes.hpp
)

SET(lib_srcs
    bpomodes.cpp
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}})

ADD_EXECUTABLE(bpo-demo main.cpp ${lib_hdrs} ${lib_srcs})
TARGET_LINK_LIBRARIES(bpo-demo ${Boost_LIBRARIES})

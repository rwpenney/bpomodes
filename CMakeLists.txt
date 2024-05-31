# CMake control-file for small-sample entropy estimator
# RW Penney, September 2023

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
SET(BPOM_VERSION "0.1.0")
PROJECT(bpo-modes)
ENABLE_TESTING()

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(GNUCC_OPTS -ansi -pedantic -Wall)
    SET(CMAKE_C_FLAGS_DEBUG:STRING "-ggdb")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

FIND_PACKAGE(Boost 1.60 REQUIRED COMPONENTS
    program_options unit_test_framework)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

SET(lib_hdrs
    bpomodes.hpp
)

SET(lib_srcs
    bpomodes.cpp
)

SET(test_srcs
    testmain.cpp
    tests.cpp
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}})

ADD_LIBRARY(bpomodes SHARED ${lib_hdrs} ${lib_srcs})
TARGET_COMPILE_FEATURES(bpomodes PRIVATE cxx_std_11)
SET_TARGET_PROPERTIES(bpomodes PROPERTIES SOVERSION "${BPOM_VERSION}")
INSTALL(TARGETS bpomodes LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib RUNTIME DESTINATION lib)
INSTALL(FILES ${lib_hdrs} DESTINATION include)

ADD_EXECUTABLE(bpo-demo demo.cpp)
TARGET_COMPILE_FEATURES(bpo-demo PRIVATE cxx_std_11)
TARGET_LINK_LIBRARIES(bpo-demo bpomodes ${Boost_LIBRARIES})

ADD_EXECUTABLE(bpo-test ${lib_hdrs} testdefns.hpp ${lib_srcs} ${test_srcs})
SET_TARGET_PROPERTIES(bpo-test
    PROPERTIES
        COMPILE_FFLAGS "-DUNIT_TESTING -DBOOST_TEST_DYN_LINK")
TARGET_LINK_LIBRARIES(bpo-test ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBSINIT})
ADD_TEST(MT bpo-test)

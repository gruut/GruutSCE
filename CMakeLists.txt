cmake_minimum_required(VERSION 3.13)
project(TethysSCE)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS unit_test_framework system thread filesystem REQUIRED)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
ENDIF (Boost_FOUND)

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(tethys-utils)

set(LIB_PREFIX "/usr/local/lib")
set(BOTAN_LIBS "${LIB_PREFIX}/libbotan-2.a")

file(GLOB SOURCE_FILES
    "src/*.cpp"
    "src/*/*.cpp"
    "include/*.cpp"
    "include/*/*.cpp"
    "tethys-utils/src/*.cpp"
    )

file(GLOB HEADER_FILES
    "src/*.hpp"
    "src/*/*.hpp"
    "include/*.hpp"
    "include/*/*.hpp"
    "tethys-utils/src/*.hpp"
    )

add_executable(TSCE main.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(TSCE PRIVATE include/ /usr/local/include)
target_link_libraries(TSCE
    PRIVATE
    ${Boost_LIBRARIES}
    ${BOTAN_LIBS}
)

add_executable(comp_test test/component_test.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(comp_test PRIVATE include/ /usr/local/include)
target_link_libraries(comp_test
    PRIVATE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_LIBRARIES}
    ${BOTAN_LIBS}
)

add_executable(run_test test/runner_test.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(run_test PRIVATE include/ /usr/local/include)
target_link_libraries(run_test
    PRIVATE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_LIBRARIES}
    ${BOTAN_LIBS}
)


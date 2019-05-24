cmake_minimum_required(VERSION 3.13)
project(VeronnSCE)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS unit_test_framework system thread filesystem)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
ENDIF (Boost_FOUND)

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/utils/gruut-utils)

set(LIB_PREFIX "/usr/local/lib")
set(BOTAN_LIBS "${LIB_PREFIX}/libbotan-2.so")

file(GLOB SOURCE_FILES
    "src/*.cpp"
    "src/*/*.cpp"
    "include/*.cpp"
    "include/*/*.cpp"
    "utils/gruut-utils/src/*.cpp"
    )

file(GLOB HEADER_FILES
    "src/*.hpp"
    "src/*/*.hpp"
    "include/*.hpp"
    "include/*/*.hpp"
    "utils/gruut-utils/src/*.hpp"
    )

add_executable(VSCE main.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(VSCE PRIVATE include/ /usr/local/include)
target_link_libraries(VSCE
    PRIVATE
    ${Boost_LIBRARIES}
    ${BOTAN_LIBS}
)

add_executable(test test/component_test.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(test PRIVATE include/ /usr/local/include)
target_link_libraries(test
    PRIVATE
    ${Boost_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${BOTAN_LIBS}
)

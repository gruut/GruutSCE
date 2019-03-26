cmake_minimum_required(VERSION 3.10)
project(GruutSCE)

option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
ENDIF (Boost_FOUND)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_PREFIX "/usr/local/lib")
set(BOTAN_LIBS "${LIB_PREFIX}/libbotan-2.so")

file(GLOB SOURCE_FILES
    "src/*.cpp"
    "src/*/*.cpp"
    "include/*.cpp"
    "include/*/*.cpp"
    )

file(GLOB HEADER_FILES
    "src/*.hpp"
    "src/*/*.hpp"
    "include/*.hpp"
    "include/*/*.hpp"
    )

add_executable(GruutSCE main.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(GruutSCE PRIVATE include/ /usr/local/include)
target_link_libraries(GruutSCE
    PRIVATE
    ${Boost_LIBRARIES}
    ${BOTAN_LIBS}
)

if(BUILD_TESTS)
    file(GLOB TEST_SOURCES test/*.cpp)

    add_executable(check ${TEST_SOURCES})
    target_include_directories(check PRIVATE include/ /usr/local/include)
    target_link_libraries(check
        PRIVATE
        ${Boost_LIBRARIES}
        ${BOTAN_LIBS}
    )
endif()
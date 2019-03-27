# CMakeList.txt : CMake project for concatfiles, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (concatfiles STATIC "src/concatfiles.cpp" "include/concatfiles.hpp")
set_property(TARGET concatfiles PROPERTY CXX_STANDARD 17)
target_include_directories(concatfiles PUBLIC "./include/")


#find_program(CLANG_CL name "clang-cl.exe" HINTS ENV "PATH")
#if (CLANG_CL)
#    message(STATUS "found clang-cl at ${CLANG_CL}")
#    set(CMAKE_CXX_COMPILER "${CLANG_CL}")
#else()
#    message(STATUS "can't find ${CLANG_CL}")
#endif()


# TODO: Add tests and install targets if needed.

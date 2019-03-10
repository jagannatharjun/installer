
set(CONCATFILES_DIR E:/Cpp/Projects/concatfiles/concatfiles)

add_subdirectory(${CONCATFILES_DIR}
                 ${CMAKE_BINARY_DIR}/concatfiles-build
                 EXCLUDE_FROM_ALL)

add_executable(compiler compiler.cpp)
target_link_libraries(compiler concatfiles)


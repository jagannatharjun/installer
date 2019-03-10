

add_executable(compiler compiler.cpp)
set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
target_link_libraries(compiler concatfiles)


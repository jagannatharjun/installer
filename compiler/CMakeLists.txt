

add_executable(compiler compiler.cpp)

add_executable(change_icon change_icon.cpp)
target_include_directories(change_icon PUBLIC E:/Cpp/Projects/Headers/Include)

set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
target_link_libraries(compiler concatfiles)


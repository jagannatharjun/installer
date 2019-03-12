
add_executable(uninstaller uninstaller.cpp)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
set_property(TARGET uninstaller PROPERTY WIN32_EXECUTABLE true)
else()
set_property(TARGET uninstaller PROPERTY WIN32_EXECUTABLE false)
endif()
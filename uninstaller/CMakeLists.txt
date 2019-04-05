
add_executable(uninstaller uninstaller.cpp)
set_property(TARGET uninstaller PROPERTY CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(uninstaller stdc++fs)
    set_property(TARGET uninstaller PROPERTY BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set_property(TARGET uninstaller PROPERTY WIN32_EXECUTABLE true)
else()
    set_property(TARGET uninstaller PROPERTY WIN32_EXECUTABLE false)
endif()

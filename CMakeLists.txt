cmake_minimum_required(VERSION 3.0.2)
project(installer)

include(cotire.cmake)
#add_subdirectory(infoware)

foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)


#add_executable(spacetest filesystemSpacetest.cpp)
#set_property(TARGET spacetest PROPERTY CXX_STANDARD 17)
#target_link_libraries(spacetest wbemuuid)
#if(CMAKE_COMPILER_IS_GNUCC)
#    target_link_libraries(spacetest stdc++fs)
#endif()

add_subdirectory(concatfiles)
add_subdirectory(compiler)
add_subdirectory(uninstaller)

find_package(Qt5 COMPONENTS Quick Core Qml QuickWidgets Multimedia REQUIRED)
foreach(c Core Sql Widgets Quick Multimedia Qml)
    if(${Qt5${c}_FOUND})
        message(STATUS "Qt5${c} found! ")
    endif()
endforeach()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
option(COMPILE_QML "Compile QML" ON)


set(CMAKE_AUTOSTATICPLUGINS ON)

find_program(CLANG_CL name "clang-cl.exe" HINTS ENV "PATH")
if (CLANG_CL)
   # message(STATUS "found clang-cl at ${CLANG_CL}")
   # set(CMAKE_CXX_COMPILER "${CLANG_CL}")
else()
    message(STATUS "can't find ${CLANG_CL}")
endif()

find_package(Qt5QuickCompiler REQUIRED)
qtquick_compiler_add_resources(RESOURCES qml.qrc)

add_executable(installer qml.qrc wmi_object.h wmi_object.cpp debug.h buf_io.h main.cpp shortcut.hpp shortcut.cpp tinstallerinfo.cpp tinstallerinfo.hpp tinstallermanager.cpp tinstallermanager.hpp TResources.cpp TResources.hpp ${RESOURCES})
set_property(TARGET installer PROPERTY WIN32_EXECUTABLE ${CMAKE_BUILD_TYPE} == "Release")
qt5_use_modules(installer Quick Core Qml QuickWidgets Multimedia)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(installer PRIVATE -DQML_DEBUG)
endif()
target_link_libraries(installer concatfiles PowrProf.lib wbemuuid.lib)
target_include_directories(installer PRIVATE "E:/Cpp/Projects/Headers/Include")
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(installer stdc++fs)
endif()
message("${CMAKE_BINARY_DIR}")
add_custom_command(TARGET installer POST_BUILD MAIN_DEPENDENCY DEPENDS compiler uninstaller COMMAND "compiler/compiler.exe" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

##cotire(mainwindow)

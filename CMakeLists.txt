cmake_minimum_required(VERSION 3.0.2)
project(installer)

include(cotire.cmake)
add_subdirectory(infoware)


#add_executable(spacetest filesystemSpacetest.cpp)
#set_property(TARGET spacetest PROPERTY CXX_STANDARD 17)
#target_link_libraries(spacetest wbemuuid)
#if(CMAKE_COMPILER_IS_GNUCC)
#    target_link_libraries(spacetest stdc++fs)
#endif()

set(CONCATFILES_DIR E:/Cpp/Projects/concatfiles/concatfiles)

add_subdirectory(${CONCATFILES_DIR}
                 ${CMAKE_BINARY_DIR}/concatfiles-build
                 EXCLUDE_FROM_ALL)

add_subdirectory(compiler)

find_package(Qt5 COMPONENTS Quick Core Qml QuickWidgets REQUIRED)
foreach(c Core Sql Widgets Quick Multimedia Qml)
    if(${Qt5${c}_FOUND})
        message(STATUS "Qt5${c} found! ")
    endif()
endforeach()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
option(COMPILE_QML "Compile QML" OFF)

find_program(CLANG_CL name "clang-cl.exe" HINTS ENV "PATH")
if (CLANG_CL)
   # message(STATUS "found clang-cl at ${CLANG_CL}")
   # set(CMAKE_CXX_COMPILER "${CLANG_CL}")
else()
    message(STATUS "can't find ${CLANG_CL}")
endif()

if (COMPILE_QML)
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES qml.qrc)
endif()

add_executable(mainwindow debug.h main.cpp shortcut.hpp shortcut.cpp tinstallerinfo.cpp tinstallerinfo.hpp tinstallermanager.cpp tinstallermanager.hpp TResources.cpp TResources.hpp ${RESOURCES})
qt5_use_modules(mainwindow Quick Core Qml QuickWidgets)
target_link_libraries(mainwindow infoware concatfiles)
target_include_directories(mainwindow PRIVATE "E:/Cpp/Projects/Headers/Include")
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(mainwindow stdc++fs)
endif()

##cotire(mainwindow)

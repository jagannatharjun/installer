add_executable(compiler compiler.cpp)
target_include_directories(compiler PUBLIC E:/Cpp/Projects/Headers/Include)
message(${CMAKE_BUILD_TYPE})
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(INSTALLER_EXE_PATH "installer_res.exe")
    set(OUT_INSTALLER_EXE "installer.exe")
else()
    set(INSTALLER_EXE_PATH "${CMAKE_BINARY_DIR}/installer.exe")
    set(OUT_INSTALLER_EXE "${CMAKE_BINARY_DIR}/installer.exe")
endif()

message("${INSTALLER_EXE_PATH} -- INSTALLER_EXE_PATH")
message("${OUT_INSTALLER_EXE} -- OUT_INSTALLER_EXE ")

target_compile_definitions(compiler PRIVATE -DINSTALLER_EXE_PATH="${INSTALLER_EXE_PATH}" -DOUT_INSTALLER_EXE="${OUT_INSTALLER_EXE}")

#add_executable(change_icon change_icon.cpp)
#target_include_directories(change_icon PUBLIC E:/Cpp/Projects/Headers/Include)

set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
target_link_libraries(compiler concatfiles)


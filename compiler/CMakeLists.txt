add_executable(compiler compiler.cpp)
target_include_directories(compiler PUBLIC E:/Cpp/Projects/Headers/Include)
message(${CMAKE_BUILD_TYPE})
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(INSTALLER_EXE_PATH "installer_res.exe")
    set(OUT_INSTALLER_EXE "installer.exe")
    set(RESOURCES_FOLDER "Resources")
    set(UNINSTALLER_EXE "res/uninstaller.exe")
else()
    set(INSTALLER_EXE_PATH "${CMAKE_BINARY_DIR}/installer.exe")
    set(OUT_INSTALLER_EXE "${CMAKE_BINARY_DIR}/installer.exe")
    set(RESOURCES_FOLDER "E:/Cpp/Projects/Gui/installer/build/Deploy/Resources")
    set(UNINSTALLER_EXE "${CMAKE_BINARY_DIR}/uninstaller/uninstaller.exe")
endif()

message("${INSTALLER_EXE_PATH} -- INSTALLER_EXE_PATH")
message("${OUT_INSTALLER_EXE} -- OUT_INSTALLER_EXE ")

target_compile_definitions(compiler PRIVATE -DINSTALLER_EXE_PATH="${INSTALLER_EXE_PATH}" -DOUT_INSTALLER_EXE="${OUT_INSTALLER_EXE}" -DRESOURCES_FOLDER="${RESOURCES_FOLDER}" -DUNINSTALLER_EXE="${UNINSTALLER_EXE}")

#add_executable(change_icon change_icon.cpp)
#target_include_directories(change_icon PUBLIC E:/Cpp/Projects/Headers/Include)

set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
target_link_libraries(compiler concatfiles)


#-------------------------------------------------
#
# Project created by QtCreator 2019-01-30T18:02:31
#
#-------------------------------------------------

QT       += qml quickcontrols2 core multimedia winextras
CONFIG   += qtquickcompiler

TARGET = installer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z
win32-g++: LIBS += -lstdc++fs -lopengl32 -lgdi32 -lversion -lOle32 -lOleAut32 -lwbemuuid -lPowrProf
win32-g++: QMAKE_CXXFLAGS += -march=native
win32-g++: DEFINES += INFOWARE_USE_OPENGL
win32-msvc: LIBS += dxgi.lib gdi32.lib version.lib Ole32.lib OleAut32.lib wbemuuid.lib PowrProf.lib
win32-msvc: DEFINES += INFOWARE_USE_D3D

SOURCES +=  main.cpp shortcut.cpp tinstallerinfo.cpp  tinstallermanager.cpp TResources.cpp 
RESOURCES +=  qml.qrc 
HEADERS += debug.h shortcut.hpp tinstallerinfo.hpp tinstallermanager.hpp TResources.hpp

SOURCES += E:/Cpp/Projects/concatfiles/concatfiles/src/concatfiles.cpp

SOURCES += infoware\src\cpu\architecture\architecture_non_windows.cpp \
infoware\src\cpu\architecture\architecture_windows.cpp \
infoware\src\cpu\endianness\all.cpp \
infoware\src\cpu\frequency\frequency_non_windows.cpp \
infoware\src\cpu\frequency\frequency_windows.cpp \
infoware\src\cpu\instuction_set\instruction_set.cpp \
infoware\src\cpu\instuction_set\instruction_set_non_windows.cpp \
infoware\src\cpu\instuction_set\instruction_set_windows.cpp \
infoware\src\cpu\quantities_cache\quantities_cache_non_windows.cpp \
infoware\src\cpu\quantities_cache\quantities_cache_windows.cpp \
infoware\src\cpu\vendor_model_name\vendor_id.cpp \
infoware\src\cpu\vendor_model_name\vendor_model_name_non_windows.cpp \
infoware\src\cpu\vendor_model_name\vendor_model_name_windows.cpp \
infoware\src\detail\cpuid.cpp \
infoware\src\detail\pci.generated.cpp \
infoware\src\detail\scope.cpp \
infoware\src\gpu\memory\blank_all.cpp \
infoware\src\gpu\memory\d3d.cpp \
infoware\src\gpu\memory\OpenCL.cpp \
infoware\src\gpu\memory\OpenGL.cpp \
infoware\src\system\amounts\amounts_non_windows.cpp \
infoware\src\system\amounts\windows.cpp \
infoware\src\system\displays\displays_default_blank.cpp \
infoware\src\system\displays\displays_windows.cpp \
infoware\src\system\displays\displays_x11.cpp \
infoware\src\system\kernel_info\kernel_info_non_windows.cpp \
infoware\src\system\kernel_info\kernel_info_windows.cpp \
infoware\src\system\memory\memory_non_windows.cpp \ 
infoware\src\system\memory\memory_windows.cpp \
infoware\src\system\OS_info\os_info_non_windows.cpp \
infoware\src\system\OS_info\os_info_windows.cpp 

INCLUDEPATH += "E:/Cpp/Projects/Headers/Include"
INCLUDEPATH += "infoware/include"
INCLUDEPATH += E:/Cpp/Projects/concatfiles/concatfiles/include



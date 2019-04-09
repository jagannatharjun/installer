#-------------------------------------------------
#
# Project created by QtCreator 2019-01-30T18:02:31
#
#-------------------------------------------------

QT       += qml quickcontrols2 core winextras
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

win32-msvc: LIBS += PowrProf.lib
win32-clang-msvc: LIBS += PowrProf.lib wbemuuid.lib
win32-clang-g++ : LIBS += -lPowrProf -lstdc++fs -lOle32 -luuid -lwbemuuid -lWinmm
win32-g++ : LIBS += -lPowrProf -lstdc++fs -lOle32 -luuid -lwbemuuid -lWinmm
RESOURCES += resources.rc
SOURCES +=  main.cpp shortcut.cpp tinstallerinfo.cpp  tinstallermanager.cpp TResources.cpp wmi_object.cpp
RESOURCES +=  qml.qrc 
HEADERS += debug.h shortcut.hpp tinstallerinfo.hpp tinstallermanager.hpp TResources.hpp wmi_object.h

SOURCES += concatfiles/src/concatfiles.cpp

INCLUDEPATH += "E:/Cpp/Projects/Headers/Include"
INCLUDEPATH += concatfiles/include



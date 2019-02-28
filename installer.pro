#-------------------------------------------------
#
# Project created by QtCreator 2019-01-30T18:02:31
#
#-------------------------------------------------

QT       += gui qml quick widgets core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
win32-g++: LIBS += -lstdc++fs
win32-cl: LIBS += "user32.lib"

SOURCES += \
        main.cpp \
    tinstallermanager.cpp \
    TResources.cpp  \
    tinstallerinfo.cpp

RESOURCES =  qml.qrc

HEADERS += \
    tinstallermanager.hpp \
    TResources.hpp \
    tinstallerinfo.hpp

INCLUDEPATH += "E:/Cpp/Projects/Headers/Include"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

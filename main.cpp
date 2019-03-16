#include "TResources.hpp"
#include "tinstallerinfo.hpp"
#include "tinstallermanager.hpp"

#include "debug.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <memory>

int main(int argc, char *argv[]) {
  int i;
  {
    qputenv("QML_DISABLE_DISTANCEFIELD", "1");

    auto Resources =
        std::make_shared<TResources>("E:/Cpp/Projects/Gui/installer/Resources");

    QGuiApplication a(argc, argv);

    SHOW(qmlRegisterType<TInstallerInfo>("Ascent.info", 1, 0, "Installerinfo"));
    Resources->setMainIni("Setup.ini");
    Resources->extractTemporaryFiles("dll/*");

    TInstallerInfo::setResources(Resources);
    TInstallerManager installer(Resources);

    i = a.exec();
  }
  return i;
}

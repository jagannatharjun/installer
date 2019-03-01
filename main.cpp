#include "TResources.hpp"
#include "tinstallerinfo.hpp"
#include "tinstallermanager.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QQmlEngine>
#include <memory>

#define SHOW(X) qDebug() << #X << ": " << (X);

int main(int argc, char *argv[]) {
  qputenv("QML_DISABLE_DISTANCEFIELD", "1");

  auto Resources =
      std::make_shared<TResources>("E:/Cpp/Projects/Gui/installer/Resources");

  QGuiApplication a(argc, argv);

  SHOW(qmlRegisterType<TInstallerInfo>("Ascent.info", 1, 0, "Installerinfo"));
  Resources->setMainIni("Setup.ini");
  TInstallerInfo::setResources(Resources);
  TInstallerManager installer(Resources);

  return a.exec();
}

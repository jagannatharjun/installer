#include "TResources.hpp"
#include "tinstallerinfo.hpp"
#include "tinstallermanager.hpp"

#include "debug.h"
#include <QGuiApplication>
#include <QIcon>
#include <QQmlEngine>
#include <memory>

//#include <QQmlDebuggingEnabler>

// QQmlDebuggingEnabler enabler;

auto Resources =
		std::make_shared<TResources>("E:/Cpp/Projects/Gui/installer/Resources");

int main(int argc, char *argv[]) {
  int i;	
  {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    QGuiApplication a(argc, argv);

    SHOW(qmlRegisterType<TInstallerInfo>("Ascent.info", 1, 0, "Installerinfo"));
    Resources->setMainIni("Setup.ini");
    Resources->extractTemporaryFile("icon.ico");
    Resources->extractTemporaryFiles("dll/*");
    Resources->extractTemporaryFiles("private/*.ttf");

    TInstallerInfo::setResources(Resources);
    a.setWindowIcon(
        QIcon((Resources->TmpFolder() / "icon.ico").string().c_str()));
    TInstallerManager installer(Resources);

    i = a.exec();
  }
  return i;
}

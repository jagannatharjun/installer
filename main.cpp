#include "TResources.hpp"
#include "tinstallerinfo.hpp"
#include "tinstallermanager.hpp"

#include "debug.h"
#include <QGuiApplication>
#include <QIcon>
#include <QQmlEngine>
#include <memory>

#include <windows.h>
//#include <QQmlDebuggingEnabler>

// QQmlDebuggingEnabler enabler;

int main(int argc, char *argv[]) try {

  if (argc == 2 && !strcmp(argv[1], "log")) {
    std::freopen("log.txt", "w", stderr);
    std::freopen("log2.txt", "w", stdout);
  }

  auto Resources = std::make_shared<TResources>("E:/Cpp/Projects/Gui/installer/Resources");
  srand(time(NULL));
  int i;
  {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication a(argc, argv);

    // SHOW(qmlRegisterType<TInstallerInfo>("Ascent.info", 1, 0, "Installerinfo"));
    Resources->setMainIni("Setup.ini");
    Resources->extractTemporaryFile("icon.ico");
    Resources->extractTemporaryFiles("dll/*");
    Resources->extractTemporaryFiles("private/*.ttf");

    TInstallerInfo::setResources(Resources);
    a.setWindowIcon(QIcon((Resources->TmpFolder() / "icon.ico").string().c_str()));
    TInstallerManager installer(Resources);

    i = a.exec();
  }
  return i;
} catch (std::exception &e) {
  MessageBoxA(NULL, (std::string("Caught Unhandled Exception: ") + e.what()).c_str(), "Error",
              MB_OK);
} catch (...) {
  MessageBoxA(NULL, "Caught Exception of Illegal Type", "Error", MB_OK);
}

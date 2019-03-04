#include "tinstallermanager.hpp"
#include "TResources.hpp"
#include "debug.h"
#include "tinstallerinfo.hpp"

#include <QDebug>
#include <QFileSystemWatcher>

#include <QMouseEvent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickImageProvider>
#include <QQuickItem>
#include <QQuickView>

#include <algorithm>
#include <memory>

class TResourcesImageProvider : public QQuickImageProvider {
public:
  TResourcesImageProvider(std::shared_ptr<TResources> Resources)
      : Resources{Resources}, QQuickImageProvider(QQuickImageProvider::Pixmap) {
  }

  QPixmap requestPixmap(const QString &id, QSize *size,
                        const QSize &requestedSize) override {

    std::vector<uint8_t> buf;

    QPixmap pixmap;
    buf.resize(0);
    Resources->GetFile(id.toStdString(), buf);
    pixmap.loadFromData(buf.data(), buf.size());

    if (size)
      *size = QSize(pixmap.width(), pixmap.height());

    qDebug() << __func__ << ": " << id << " " << buf.size();
    if (buf.size() == 0) {
      qDebug() << "failed to read " << id;
    }
    return pixmap;
  }

private:
  std::shared_ptr<TResources> Resources;
};

TInstallerManager::TInstallerManager(std::shared_ptr<TResources> Resources)
    : Resources_(Resources) {

  quickView = new QQuickView();
  quickView->installEventFilter(this);

  auto engine = quickView->engine();
  engine->addImageProvider("resources", new TResourcesImageProvider(Resources));
  quickView->setFlag(Qt::FramelessWindowHint);

  QQmlContext *rootCtx = quickView->rootContext();
  SHOW(rootCtx);

  // take ownership of packs so that their destruction will be guarrenteed
  std::for_each(TInstallerInfo::languagePack.begin(),
                TInstallerInfo::languagePack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::componentsPack.begin(),
                TInstallerInfo::componentsPack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::redestribPack.begin(),
                TInstallerInfo::redestribPack.end(),
                [this](auto& o) { o->setParent(this); });

  rootCtx->setContextProperty(
      "languagePackModel", QVariant::fromValue(TInstallerInfo::languagePack));
  rootCtx->setContextProperty(
      "componentPackModel",
      QVariant::fromValue(TInstallerInfo::componentsPack));
  rootCtx->setContextProperty(
      "redistPackModel", QVariant::fromValue(TInstallerInfo::redestribPack));
  // rootCtx->setContextProperty("installer_info", InstallerInfo_);

  loadMainQML();
  quickView->show();

  auto t = new QFileSystemWatcher(this);
  t->addPath(R"(E:\Cpp\Projects\Gui\installer)");
  auto timeout = [&]() { this->loadMainQML(); };
  QObject::connect(t, &QFileSystemWatcher::fileChanged, timeout);
  QObject::connect(t, &QFileSystemWatcher::directoryChanged, timeout);
}

TInstallerManager::~TInstallerManager() { delete quickView; }

bool TInstallerManager::eventFilter(QObject *, QEvent *e) {
  if (e->type() == QEvent::MouseButtonPress) {
    qDebug() << __func__ << ": MouseButtonPress";
    ReleaseCapture();
    SendMessage((HWND)quickView->winId(), 0x0112, 0xF012, 0);
  }
  return false;
}

void TInstallerManager::loadMainQML() {
  quickView->setSource(QUrl());
  quickView->engine()->clearComponentCache();
  quickView->setSource(
      QUrl::fromLocalFile(R"(E:\Cpp\Projects\Gui\installer\Main.qml)"));
  QQuickItem *root = quickView->rootObject();

  SHOW(QObject::connect(root, SIGNAL(websiteButtonClicked()), this,
                        SLOT(websiteButtonClicked())));

  SHOW(QObject::connect(root, SIGNAL(facebookButtonClicked()), this,
                        SLOT(facebookButtonClicked())));

  SHOW(QObject::connect(root, SIGNAL(visitThreadButtonClicked()), this,
                        SLOT(visitThreadButtonClicked())));

  SHOW(QObject::connect(root, SIGNAL(minimizeButtonClicked()), this,
                        SLOT(minimizeButtonClicked())));

  SHOW(QObject::connect(root, SIGNAL(closeButtonClicked()), this,
                        SLOT(closeButtonClicked())));

  SHOW(QObject::connect(root, SIGNAL(musicButtonClicked()), this,
                        SLOT(musicButtonClicked())));
}

void TInstallerManager::websiteButtonClicked() { qDebug() << __func__; }

void TInstallerManager::facebookButtonClicked() { qDebug() << __func__; }

void TInstallerManager::visitThreadButtonClicked() { qDebug() << __func__; }

void TInstallerManager::minimizeButtonClicked() {
  qDebug() << __func__;
  quickView->setWindowStates(quickView->windowStates() | Qt::WindowMinimized);
}

void TInstallerManager::closeButtonClicked() {
  qDebug() << __func__;
  TInstallerInfo::setTerminateInstallation(true);
  quickView->close();
}

void TInstallerManager::musicButtonClicked() { qDebug() << __func__; }

void TInstallerManager::loadPageQml(const QUrl &qmlFile) {}

#include "tinstallermanager.hpp"
#include "TResources.hpp"
#include "debug.h"
#include "tinstallerinfo.hpp"

#include <QQmlContext>
#include <QQuickImageProvider>

#include <QDebug>
#include <algorithm>
#include <memory>

#include <windows.h>

class TResourcesImageProvider : public QQuickImageProvider {
public:
  TResourcesImageProvider(std::shared_ptr<TResources> Resources)
      : Resources{Resources}, QQuickImageProvider(QQuickImageProvider::Image) {}
  QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    auto imagebuf = Resources->GetFile(id.toStdString());
    auto imagearray =
        QByteArray::fromRawData(reinterpret_cast<const char *>(imagebuf.data()), imagebuf.size());
    SHOW(id);
    SHOW(imagearray.size());
    auto image = QImage::fromData(imagearray);
    SHOW(image.isNull());
    if (size)
      *size = image.size();

    return image;
  }
  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override {
    debug(__func__);
    std::vector<uint8_t> buf;

    QPixmap pixmap;
    buf.resize(0);
    Resources->GetFile(id.toStdString(), buf);
    SHOW(pixmap.loadFromData(buf.data(), buf.size()));

    if (size)
      *size = QSize(pixmap.width(), pixmap.height());

    qDebug() << __func__ << ": " << id << " " << buf.size();

    if (buf.size() == 0) {
      //      qDebug() << "failed to read " << id.toStdString();
    }
    return pixmap;
  }

private:
  std::shared_ptr<TResources> Resources;
};

TInstallerManager::TInstallerManager(std::shared_ptr<TResources> Resources)
    : Resources_(Resources) {

  auto music_mp3 = Resources_->extractTemporaryFile("music.mp3").string();
  auto r = mciSendStringA(gupta::format(R"(open "%" type mpegvideo alias mp3)", music_mp3).c_str(),
                          NULL, 0, NULL);
  if (r != 0) {
    char c[1024];
    mciGetErrorStringA(r, c, sizeof c);
    debug("mci failed %", c);
  }
  r = mciSendStringA("play mp3 repeat", NULL, 0, NULL);
  if (r != 0) {
    char c[1024];
    mciGetErrorStringA(r, c, sizeof c);
    debug("mci failed %", c);
  }

  InstallerInfo_ = new TInstallerInfo(this);

  imageProvider = new TResourcesImageProvider(Resources);
  Application_.addImageProvider("resources", imageProvider);

  QQmlContext *rootCtx = Application_.rootContext();
  SHOW(rootCtx);

  // take ownership of packs so that their destruction will be guarrenteed
  std::for_each(TInstallerInfo::languagePack.begin(), TInstallerInfo::languagePack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::componentsPack.begin(), TInstallerInfo::componentsPack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::redestribPack.begin(), TInstallerInfo::redestribPack.end(),
                [this](auto &o) { o->setParent(this); });

  TInstallerInfo::desktopShortcut = new TComponent("desktopShortcut", true, this);
  TInstallerInfo::startMenuShortcut = new TComponent("startMenuShortcut", true, this);

  rootCtx->setContextProperty("languagePackModel",
                              QVariant::fromValue(TInstallerInfo::languagePack));
  rootCtx->setContextProperty("componentPackModel",
                              QVariant::fromValue(TInstallerInfo::componentsPack));
  rootCtx->setContextProperty("redistPackModel",
                              QVariant::fromValue(TInstallerInfo::redestribPack));
  rootCtx->setContextProperty("desktopShortcut", (TInstallerInfo::desktopShortcut));
  rootCtx->setContextProperty("startMenuShortcut", (TInstallerInfo::startMenuShortcut));
  rootCtx->setContextProperty("installer_info", InstallerInfo_);
  // rootCtx->setContextProperty("installer_info", InstallerInfo_);

  Application_.load("qrc:/Main.qml");
  QObject *rootObject = Application_.rootObjects().first();

  connect(rootObject, SIGNAL(musicButtonClicked()), this, SLOT(musicButtonClicked()));

  /*auto t = new QFileSystemWatcher(this);
  t->addPath(R"(E:\Cpp\Projects\Gui\installer)");
  auto timeout = [&]() {
    Application_.quit();
    Application_.load("qrc:/Main.qml");
  };
  QObject::connect(t, &QFileSystemWatcher::fileChanged, timeout);
  QObject::connect(t, &QFileSystemWatcher::directoryChanged, timeout);*/
}

TInstallerManager::~TInstallerManager() {
  mciSendStringA("close mp3", NULL, 0, 0);
  // delete imageProvider; // engine will take ownership
}

void TInstallerManager::musicButtonClicked() {
	static bool isMusicPlaying = true;
	if (isMusicPlaying) {
		mciSendStringA("pause mp3",NULL, 0, NULL);
		isMusicPlaying = false;
	} else {
		mciSendStringA("resume mp3",NULL,0,NULL);
		isMusicPlaying = true;
	}
}

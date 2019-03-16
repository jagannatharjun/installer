#include "tinstallermanager.hpp"
#include "TResources.hpp"
#include "debug.h"
#include "tinstallerinfo.hpp"

#include <QQmlContext>
#include <QQuickImageProvider>

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <algorithm>
#include <memory>

class TResourcesImageProvider : public QQuickImageProvider {
public:
  TResourcesImageProvider(std::shared_ptr<TResources> Resources)
      : Resources{Resources}, QQuickImageProvider(QQuickImageProvider::Image) {}
  QImage requestImage(const QString &id, QSize *size,
                      const QSize &requestedSize) {
    auto imagebuf = Resources->GetFile(id.toStdString());
    auto imagearray = QByteArray::fromRawData(
        reinterpret_cast<const char *>(imagebuf.data()), imagebuf.size());
    SHOW(id);
    SHOW(imagearray.size());
    auto image = QImage::fromData(imagearray);
    SHOW(image.isNull());
    if (size)
      *size = image.size();

    return image;
  }
  QPixmap requestPixmap(const QString &id, QSize *size,
                        const QSize &requestedSize) override {
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
      qDebug() << "failed to read " << id;
    }
    return pixmap;
  }

private:
  std::shared_ptr<TResources> Resources;
};

TInstallerManager::TInstallerManager(std::shared_ptr<TResources> Resources)
    : Resources_(Resources) {

  imageProvider = new TResourcesImageProvider(Resources);
  Application_.addImageProvider("resources", imageProvider);

  QQmlContext *rootCtx = Application_.rootContext();
  SHOW(rootCtx);

  Player_ = new QMediaPlayer(this);
  /*connect(player, SIGNAL(positionChanged(qint64)), this,
          SLOT(positionChanged(qint64)));*/
  Resources_->extractTemporaryFile("music.mp3");
  auto playlist = new QMediaPlaylist(this);
  playlist->addMedia(QUrl::fromLocalFile(QString::fromStdWString(
      Resources_->extractTemporaryFile("music.mp3").wstring())));
  playlist->setPlaybackMode(QMediaPlaylist::Loop);
  Player_->setMedia(playlist);
  Player_->setVolume(50);
  Player_->play();

  // take ownership of packs so that their destruction will be guarrenteed
  std::for_each(TInstallerInfo::languagePack.begin(),
                TInstallerInfo::languagePack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::componentsPack.begin(),
                TInstallerInfo::componentsPack.end(),
                [this](auto &o) { o->setParent(this); });
  std::for_each(TInstallerInfo::redestribPack.begin(),
                TInstallerInfo::redestribPack.end(),
                [this](auto &o) { o->setParent(this); });

  TInstallerInfo::desktopShortcut =
      new TComponent("desktopShortcut", true, this);
  TInstallerInfo::startMenuShortcut =
      new TComponent("startMenuShortcut", true, this);

  rootCtx->setContextProperty(
      "languagePackModel", QVariant::fromValue(TInstallerInfo::languagePack));
  rootCtx->setContextProperty(
      "componentPackModel",
      QVariant::fromValue(TInstallerInfo::componentsPack));
  rootCtx->setContextProperty(
      "redistPackModel", QVariant::fromValue(TInstallerInfo::redestribPack));
  rootCtx->setContextProperty("desktopShortcut",
                              (TInstallerInfo::desktopShortcut));
  rootCtx->setContextProperty("startMenuShortcut",
                              (TInstallerInfo::startMenuShortcut));
  // rootCtx->setContextProperty("installer_info", InstallerInfo_);

  Application_.load("qrc:/Main.qml");
  QObject *rootObject = Application_.rootObjects().first();

  connect(rootObject, SIGNAL(musicButtonClicked()), this,
          SLOT(musicButtonClicked()));

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
  // delete imageProvider; // engine will take ownership
}

void TInstallerManager::musicButtonClicked() {
  if (Player_->state() == QMediaPlayer::PausedState)
    Player_->play();
  else
    Player_->pause();
}

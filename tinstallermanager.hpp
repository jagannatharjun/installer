#ifndef TINSTALLERMANAGER_HPP
#define TINSTALLERMANAGER_HPP

#include <QQmlApplicationEngine>
#include <memory>

class QQuickView;
class TResources;
class QQuickImageProvider;
class QMediaPlayer;

class TInstallerManager : public QObject {
  Q_OBJECT
public:
  explicit TInstallerManager(std::shared_ptr<TResources> Resources);
  virtual ~TInstallerManager();

signals:

private:
  QQmlApplicationEngine Application_;
  QQuickImageProvider *imageProvider;
  std::shared_ptr<TResources> Resources_;
  QMediaPlayer *Player_;

private slots:
  void musicButtonClicked();
};

#endif // TINSTALLERMANAGER_HPP

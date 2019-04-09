#ifndef TINSTALLERMANAGER_HPP
#define TINSTALLERMANAGER_HPP

#include <QQmlApplicationEngine>
#include <memory>

class QQuickView;
class TResources;
class QQuickImageProvider;
class TInstallerInfo;

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
  TInstallerInfo *InstallerInfo_;

private slots:
  void musicButtonClicked();
};

#endif // TINSTALLERMANAGER_HPP

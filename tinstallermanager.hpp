#ifndef TINSTALLERMANAGER_HPP
#define TINSTALLERMANAGER_HPP

#include <QMainWindow>
#include <memory>

class QQuickView;
class TResources;
class TInstallerInfo;

class TInstallerManager : public QObject {
  Q_OBJECT
public:
  explicit TInstallerManager(std::shared_ptr<TResources> Resources);
  virtual ~TInstallerManager();
  bool eventFilter(QObject *, QEvent *e);

signals:

public slots:
  void websiteButtonClicked();
  void facebookButtonClicked();
  void visitThreadButtonClicked();
  void minimizeButtonClicked();
  void closeButtonClicked();
  void musicButtonClicked();
  void loadMainQML();

private:
  QQuickView *quickView;
  std::shared_ptr<TResources> Resources_;
  TInstallerInfo *InstallerInfo_;
  void loadPageQml(const QUrl &qmlFile);
};

#endif // TINSTALLERMANAGER_HPP

#ifndef TINSTALLERINFO_HPP
#define TINSTALLERINFO_HPP

#include <QColor>
#include <QObject>
#include <QString>
#include <QVector>
#include <filesystem>
#include <memory>
#include <thread>

class TResources;

class TComponent : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);
  Q_PROPERTY(
      bool checked MEMBER checked WRITE setChecked NOTIFY checkedChanged);
signals:
  void nameChanged();
  void checkedChanged();

public:
  QString name;
  bool checked;

  void setChecked(bool);
  TComponent(QString s, bool Checked = false, QObject *parent = nullptr)
      : QObject(parent), name{std::move(s)}, checked{Checked} {}

  using QObject::QObject;
};

class TInstallerInfo : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString applicationName READ applicationName NOTIFY
                 applicationNameChanged)
  Q_PROPERTY(QString applicationDescription READ applicationDescription NOTIFY
                 appDescChanged);
  Q_PROPERTY(QString destinationFolder READ destinationFolder WRITE
                 setDestinationFolder NOTIFY destinationFolderChanged);

  Q_PROPERTY(QColor themeColor READ themeColor NOTIFY themeColorChanged);
  Q_PROPERTY(int requiredSize READ requiredSize NOTIFY sizeStatsChanged);
  Q_PROPERTY(int diskTotalSpace READ diskTotalSpace NOTIFY sizeStatsChanged);
  Q_PROPERTY(int diskFreeSpace READ diskFreeSpace NOTIFY sizeStatsChanged);

public:
  using ResourcePtr = std::shared_ptr<TResources>;
  explicit TInstallerInfo(QObject *parent = nullptr);
  ~TInstallerInfo();

  QString applicationName();
  QString applicationDescription();
  QColor themeColor();
  int requiredSize();
  int diskTotalSpace();
  int diskFreeSpace();

  Q_INVOKABLE QString info(const QString &);
  Q_INVOKABLE QString requirement(const QString &);
  Q_INVOKABLE QString osText();
  Q_INVOKABLE QString buildNumberText();
  Q_INVOKABLE QString cpuText();
  Q_INVOKABLE QString cpuFrequencyText();
  Q_INVOKABLE int ramSize();
  Q_INVOKABLE int ramUsage();
  Q_INVOKABLE QString gpuText();
  Q_INVOKABLE int cpuUsage();
  Q_INVOKABLE QString expandConstant(QString);
  Q_INVOKABLE void startInstallation();

  void setDestinationFolder(const QString &destinationFolder);
  QString destinationFolder() const;

  static void setResources(ResourcePtr p);
  static void setTerminateInstallation(bool t) { terminateInstallation_ = t; }
  static bool terminateInstallation() { return terminateInstallation_; }
  static auto resources() { return Resources; }
  static QList<QObject *> componentsPack, languagePack;

signals:
  void applicationNameChanged();
  void appDescChanged();
  void releaseSizeChanged();
  void themeColorChanged();
  void sizeStatsChanged();
  void destinationFolderChanged();

  void installationFailed(QString msg);
  void installationCompleted(QString msg);
public slots:

private:
  static ResourcePtr Resources;
  static std::filesystem::space_info m_driveInfo;
  static QString m_destinationFolder;
  static void setDestinationFolderImpl(const QString &destinationFolder);
  static bool terminateInstallation_;

  std::unique_ptr<std::thread> installerThread_;
  bool installerRunning_ = false;
  void startInstallationImpl();
};

#endif // TINSTALLERINFO_HPP

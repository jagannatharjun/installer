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
  virtual ~TComponent();
};

class TRedistributable : public TComponent {
public:
  std::string Cmd;
  using TComponent::TComponent;
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

  Q_PROPERTY(double progress READ progress NOTIFY progressChanged);
  // Q_PROPERTY(QString remainingTime READ remainingTime NOTIFY progressChanged)
  // Q_PROPERTY(QString totalTime READ totalTime NOTIFY progressChanged)

public:
  using ResourcePtr = std::shared_ptr<TResources>;
  explicit TInstallerInfo(QObject *parent = nullptr);
  ~TInstallerInfo();

  enum class TInstallerStates {
    InstallationNeverStarted,
    InstallationRunning,
    InstallationFinished,
    InstallationFailed,
    InstallationStopped
  };

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
  Q_INVOKABLE static QString expandConstant(QString);
  double progress();
  Q_INVOKABLE QString remainingTime() { return remainingTime_; }
  Q_INVOKABLE QString totalTime() { return totalTime_; }
  Q_INVOKABLE void startInstallation();
  Q_INVOKABLE QString threadUrl() { return threadUrl_; }
  Q_INVOKABLE QString facebookUrl() { return facebookUrl_; }
  Q_INVOKABLE QString websiteUrl() { return websiteUrl_; }
  Q_INVOKABLE static void terminateInstallation() {
    terminateInstallation_ = true;
    // installerState_ = TInstallerStates::InstallationStopped;
  }
  Q_INVOKABLE static bool isTerminateInstallation() {
    return terminateInstallation_;
  }
  Q_INVOKABLE double getProgress() { return progress(); }

  void setDestinationFolder(const QString &destinationFolder);
  static QString destinationFolder();

  void setProgress(double progress);

  static void setResources(ResourcePtr p);
  static auto resources() { return Resources; }
  static QList<QObject *> componentsPack, languagePack, redestribPack;
  static TComponent *desktopShortcut, *startMenuShortcut;

signals:
  void applicationNameChanged();
  void appDescChanged();
  void releaseSizeChanged();
  void themeColorChanged();
  void sizeStatsChanged();
  void destinationFolderChanged();
  void progressChanged();

  void installationFailed(QString msg);
  void installationCompleted(QString msg);
public slots:

private:
  static ResourcePtr Resources;
  static std::filesystem::space_info m_driveInfo;
  static QString m_destinationFolder;
  static void setDestinationFolderImpl(const QString &destinationFolder);
  static bool terminateInstallation_;
  static QString websiteUrl_, facebookUrl_, threadUrl_;

  double Progress_ = 0;
  std::thread installerThread_;
  bool installerRunning_ = false;
  QString remainingTime_, totalTime_;
  TInstallerStates installerState_ = TInstallerStates::InstallationNeverStarted;
  void startInstallationImpl();
};

#endif // TINSTALLERINFO_HPP

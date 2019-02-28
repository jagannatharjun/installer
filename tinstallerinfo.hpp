#ifndef TINSTALLERINFO_HPP
#define TINSTALLERINFO_HPP

#include <QColor>
#include <QObject>
#include <QString>
#include <QVector>
#include <filesystem>
#include <memory>

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
  void setResources(ResourcePtr p);
  auto resources() { return Resources; }

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

  void setDestinationFolder(const QString &destinationFolder);
  QString destinationFolder() const;

  QList<QObject *> componentsPack, languagePack;

signals:
  void applicationNameChanged();
  void appDescChanged();
  void releaseSizeChanged();
  void themeColorChanged();
  void sizeStatsChanged();
  void destinationFolderChanged();

public slots:

private:
  ResourcePtr Resources;
  std::filesystem::space_info m_driveInfo;
  QString m_destinationFolder;
};

#endif // TINSTALLERINFO_HPP

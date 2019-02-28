#include "tinstallerinfo.hpp"
#include "TResources.hpp"
#include "debug.h"
#include <QColor>
#include <QDebug>
#include <algorithm>
#include <filesystem>

#include <gupta/dll.hpp>
#include <infoware/infoware.hpp>

std::string LineSection(const std::string_view line, const std::string_view str,
                        const std::string &Default = "") {
  auto l = line.find(str);
  if (l == str.npos)
    return Default;
  auto e = line.find(';', l + str.length());
  if (e == line.npos)
    e = line.length();
  return std::string(line.data() + l + str.length(), e - l - str.length());
}

bool StrToBool(std::string str) {
  std::for_each(str.begin(), str.end(), [](auto &c) { c = tolower(c); });
  return str == "true" || str == "1";
}

TComponent *ComponentFromIni(std::string str, QObject *parent) {
  auto c = new TComponent(parent);
  str.insert(0, ":");
  c->name = LineSection(str, ":").c_str();
  SHOW(LineSection(str, "Checked:"));
  c->checked = StrToBool(LineSection(str, "Checked:"));
  SHOW(c->checked);
  return c;
}

TInstallerInfo::TInstallerInfo(QObject *parent) : QObject(parent) {}

void TInstallerInfo::setResources(TInstallerInfo::ResourcePtr p) {
  if (!(Resources = std::move(p)))
    return;

  QString pfDir = qgetenv("ProgramFiles").toStdString().c_str();
  setDestinationFolder(pfDir + '\\' + applicationName());
  int i = 1;
  std::string name;
  while ((name = Resources->GetIniValue(
              "LanguagePacks", "Name" + std::to_string(i++), "")) != "") {
    languagePack.push_back(ComponentFromIni(name, this));
  }
  i = 1;
  while ((name = Resources->GetIniValue(
              "ComponentPacks", "Name" + std::to_string(i++), "")) != "") {
    componentsPack.push_back(ComponentFromIni(name, this));
  }
}

QString TInstallerInfo::applicationName() {
  // return Resources->GetIniValue("Setup", "AppName", "Failed").c_str();
  return Resources->GetIniValue("Setup", "AppName", "").c_str();
}

QString TInstallerInfo::applicationDescription() {
  qDebug() << __func__;
  static QString d;
  if (d.isEmpty()) {
    auto f = Resources->GetFileText("Description.txt");
    qDebug() << "Read " << f.size();
    d = QString::fromLocal8Bit(reinterpret_cast<const char *>(f.data()),
                               f.size());
  }
  return d;
}

QColor TInstallerInfo::themeColor() {
  // return QColor("#d03a1d");
  return QColor("#d03a1d");
}

int TInstallerInfo::requiredSize() {
  return std::stoull(Resources->GetIniValue("Setup", "ReleaseSize", ""));
}

int TInstallerInfo::diskTotalSpace() {
  return m_driveInfo.capacity / (1024 * 1024);
}

int TInstallerInfo::diskFreeSpace() { return m_driveInfo.free / (1024 * 1024); }

QString TInstallerInfo::info(const QString &s) {
  SHOW(s);
  return Resources->GetIniValue("Setup", s.toStdString(), "").c_str();
}

QString TInstallerInfo::requirement(const QString &s) {
  return QString::fromStdString(
      Resources->GetIniValue("Requirements", s.toStdString(), ""));
}

#include <thread>

static iware::system::OS_info_t i;
void initOSINFO() {
  if (i.full_name.empty()) {
    // CoInitiailize will fail in current thread
    std::thread helper([&]() { i = iware::system::OS_info(); });
    helper.join();
  }
  SHOW(i.name);
  SHOW(i.full_name);
}
QString TInstallerInfo::osText() {
  initOSINFO();
  return QString::fromStdString(i.full_name);
}

QString TInstallerInfo::buildNumberText() {
  initOSINFO();
  auto n = [](auto &&q) { return QString::number(q); };
  return n(i.patch) + '.' + n(i.build_number);
}
QString TInstallerInfo::cpuText() { return iware::cpu::model_name().c_str(); }
QString TInstallerInfo::cpuFrequencyText() {
  return QString::number(iware::cpu::max_frequency());
}
int TInstallerInfo::ramSize() {
  return iware::system::memory().physical_total / (1024 * 1024);
}

int TInstallerInfo::ramUsage() {
  auto m = iware::system::memory();
  return ((m.physical_total - m.physical_available) * 100) / m.physical_total;
}

iware::gpu::device_properties_t getGPU() {
  iware::gpu::device_properties_t prop;
  auto f = iware::gpu::device_properties();
  for (auto &t : f) {
    switch (t.vendor) {
    case iware::gpu::vendor_t::amd:
    case iware::gpu::vendor_t::nvidia:
      return t;
    }
    prop = t;
  }
  return prop;
}

QString TInstallerInfo::gpuText() { return getGPU().name.c_str(); }

int TInstallerInfo::cpuUsage() { return iware::system::cpu_usage(); }

QString TInstallerInfo::expandConstant(QString str) {
  str.replace("{app}", destinationFolder());
  str.replace("{src}", QString::fromStdU16String(
                           std::filesystem::current_path().u16string()));
  str.replace("{tmp}",
              QString::fromStdU16String(Resources->TmpFolder().u16string()));
  return str;
}

void TInstallerInfo::startInstallation() {
  gupta::DynamicLib isdonedll(
      Resources->extractTemporaryFile("dll/isdone.dll").wstring().c_str());
}

void TInstallerInfo::setDestinationFolder(const QString &destinationFolder) {
  SHOW(destinationFolder);
  m_destinationFolder = destinationFolder;
  if (m_destinationFolder.startsWith("file:///"))
    m_destinationFolder.remove(0, 8);
  m_destinationFolder.replace('/', '\\');
  SHOW(m_destinationFolder);
  std::error_code ec;
  auto f = std::filesystem::path(m_destinationFolder.toStdString()).root_name();
  m_driveInfo = std::filesystem::space(f, ec);
  SHOW(ec.message().c_str());
  SHOW(m_driveInfo.capacity);
  emit destinationFolderChanged();
  emit sizeStatsChanged();
}

QString TInstallerInfo::destinationFolder() const {
  return m_destinationFolder;
}

void TComponent::setChecked(bool s) {
  qDebug(__func__);
  checked = s;
}

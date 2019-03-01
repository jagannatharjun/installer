#include "tinstallerinfo.hpp"
#include "TResources.hpp"
#include "debug.h"
#include <QColor>
#include <QDebug>
#include <algorithm>
#include <filesystem>

#include <gupta/cleanup.hpp>
#include <gupta/dll.hpp>
#include <infoware/infoware.hpp>

#include <thread>

TInstallerInfo::ResourcePtr TInstallerInfo::Resources;
std::filesystem::space_info TInstallerInfo::m_driveInfo;
QString TInstallerInfo::m_destinationFolder;
QList<QObject *> TInstallerInfo::componentsPack, TInstallerInfo::languagePack;
bool TInstallerInfo::terminateInstallation_ = false;

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
  c->checked = StrToBool(LineSection(str, "Checked:"));
  return c;
}

TInstallerInfo::TInstallerInfo(QObject *parent) : QObject(parent) {}

TInstallerInfo::~TInstallerInfo() {
  using namespace std::chrono_literals;
  while (installerRunning_)
    std::this_thread::sleep_for(1000ms);
}

void TInstallerInfo::setResources(TInstallerInfo::ResourcePtr p) {
  if (!(Resources = std::move(p)))
    return;

  QString pfDir = qgetenv("ProgramFiles").toStdString().c_str();
  setDestinationFolderImpl(
      pfDir + '\\' + Resources->GetIniValue("Setup", "AppName", "").c_str());

  int i = 1;
  std::string name;
  while ((name = Resources->GetIniValue(
              "LanguagePacks", "Name" + std::to_string(i++), "")) != "") {
    languagePack.push_back(ComponentFromIni(name, nullptr));
  }
  i = 1;
  while ((name = Resources->GetIniValue(
              "ComponentPacks", "Name" + std::to_string(i++), "")) != "") {
    componentsPack.push_back(ComponentFromIni(name, nullptr));
  }
}

void TInstallerInfo::setDestinationFolderImpl(
    const QString &destinationFolder) {
  m_destinationFolder = destinationFolder;
  if (m_destinationFolder.startsWith("file:///"))
    m_destinationFolder.remove(0, 8);
  m_destinationFolder.replace('/', '\\');
  std::error_code ec;
  auto f = std::filesystem::path(m_destinationFolder.toStdString()).root_name();
  m_driveInfo = std::filesystem::space(f, ec);
}

QString TInstallerInfo::applicationName() {
  return Resources->GetIniValue("Setup", "AppName", "").c_str();
}

QString TInstallerInfo::applicationDescription() {
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

typedef int Number;
typedef int __stdcall cbtype(char *what, Number int1, Number int2, char *str);

int __stdcall cb(char *what, Number int1, Number int2, char *str) {
  debug("\"%\" \"%\" \"%\" \"%\"", what, (uint64_t)int1 << 20,
        (uint64_t)int2 << 20, str);
  if (TInstallerInfo::terminateInstallation())
    std::strcpy(what, "quit");
  return 1;
}

void TInstallerInfo::startInstallation() {
  if (installerThread_) {
    debug("can't start installtion twice");
    return;
  }
  installerThread_ = std::make_unique<std::thread>(
      &TInstallerInfo::startInstallationImpl, this);
  installerThread_->detach();
}

void TInstallerInfo::setDestinationFolder(const QString &destinationFolder) {
  setDestinationFolderImpl(destinationFolder);
  emit destinationFolderChanged();
  emit sizeStatsChanged();
}

QString TInstallerInfo::destinationFolder() const {
  return m_destinationFolder;
}

void TInstallerInfo::startInstallationImpl() try {
  installerRunning_ = true;
  gupta::destructor resetStatus = [&]() { installerRunning_ = false; };

  gupta::DynamicLib unarcdll(
      Resources->extractTemporaryFile("dll/unarc.dll").wstring().c_str());
  if (!unarcdll.isLoaded())
    throw std::runtime_error("failed to load unarc.dll");

  using extractor = int(__cdecl *)(cbtype *, ...);
  using unloadDll_t = void(__cdecl *)();
  auto FreeArcExtract =
      (extractor)GetProcAddress(unarcdll.module(), "FreeArcExtract");
  if (!FreeArcExtract)
    throw std::runtime_error{"failed to export FreeArcExport"};

  char *s[] = {"x", "E:/test.arc", "\0"};
  auto r = FreeArcExtract(cb, s[0], s[1], s[2], NULL);
  if (r)
    throw std::runtime_error{"FreeArcExtract Failed with error code: " +
                             std::to_string(r)};

  auto UnloadDll = (unloadDll_t)GetProcAddress(unarcdll.module(), "UnloadDLL");
  if (!UnloadDll)
    throw std::runtime_error{"failed to load UnloadDll"};
  UnloadDll();

} catch (std::exception &e) {
  emit installationFailed(e.what());
}

void TComponent::setChecked(bool s) {
  qDebug(__func__);
  checked = s;
}

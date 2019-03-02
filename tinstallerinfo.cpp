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

TInstallerInfo::~TInstallerInfo() { installerThread_.join(); }

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

using Number = int;
using cbtype = int __stdcall(char *what, Number int1, Number int2, char *str);
int LastArcError = 0;
int64_t LastArchiveSize = 0, LastFileSize = 0;
std::string LastArcErrorMsg, LastArcFile;

int __stdcall cb(char *what, Number int1, Number int2, char *str) {
  debug(R"("%" "%" "%")", what, (uint64_t)int1 << 20, (uint64_t)int2 << 20);
  if (!std::strcmp(what, "filename")) {
    LastArcFile = str;
    LastFileSize = int1 << 20;
  } else if (!std::strcmp(what, "total")) {
    LastArchiveSize = int1 << 20;
  } else if (!std::strcmp(what, "error")) {
    debug("error:%:%", int1, str);
    LastArcError = int1;
    LastArcErrorMsg = str;
    ExitThread(int1);
  }
  if (TInstallerInfo::terminateInstallation()) {
    debug("quitting installation");
    return -127;
  }
  return 1;
}

void TInstallerInfo::startInstallation() {
  debug("starting installation");
  if (installerRunning_) {
    debug("can't start installation twice");
    return;
  }
  installerThread_ = std::thread(&TInstallerInfo::startInstallationImpl, this);
}

void TInstallerInfo::setDestinationFolder(const QString &destinationFolder) {
  setDestinationFolderImpl(destinationFolder);
  emit destinationFolderChanged();
  emit sizeStatsChanged();
}

QString TInstallerInfo::destinationFolder() { return m_destinationFolder; }

struct ArcDataFile {
  std::string extractionCmd[20];
  int PackedSize = 0, Component;
  std::string FileName;
  ArcDataFile(std::string line) {
    int i = 0;
    line.insert(0, ":");
    auto insertCmd = [&](auto... c) {
      assert(i + sizeof...(c) < 20);
      ((extractionCmd[i++] = c), ...);
    };
    insertCmd("x");

    if (auto password = LineSection(line, "Password:"); !password.empty()) {
      insertCmd("-p", password);
    }

    if (auto tmpDir = LineSection(line, "Tmp:"); !tmpDir.empty()) {
      insertCmd("-w" +
                TInstallerInfo::expandConstant(tmpDir.c_str()).toStdString());
    } else {
      insertCmd("-w" + TInstallerInfo::expandConstant("{tmp}\\").toStdString());
    }

    if (auto destDir = LineSection(line, "Dest:"); !destDir.empty()) {
      insertCmd("-dp" +
                TInstallerInfo::expandConstant(destDir.c_str()).toStdString());
    } else {
      insertCmd("-dp" +
                TInstallerInfo::expandConstant("{app}\\").toStdString());
    }

    insertCmd("--", FileName = TInstallerInfo::expandConstant(
                                   LineSection(line, ":").c_str())
                                   .toStdString());
  
    Component = std::stoi(LineSection(line, "Comp:", "0"));
  }
};

void TInstallerInfo::startInstallationImpl() try {
  installerRunning_ = true;
  SCOPE_EXIT { installerRunning_ = false; };

  using extractor = int(__cdecl *)(cbtype *, ...);
  using unloadDll_t = void(__cdecl *)();

  gupta::DynamicLib unarcdll(
      Resources->extractTemporaryFile("dll/unarc.dll").wstring().c_str());
  if (!unarcdll.isLoaded())
    throw std::runtime_error("failed to load unarc.dll");

  auto FreeArcExtract =
      (extractor)GetProcAddress(unarcdll.module(), "FreeArcExtract");
  if (!FreeArcExtract)
    throw std::runtime_error{"failed to export FreeArcExport"};

  auto UnloadDll = (unloadDll_t)GetProcAddress(unarcdll.module(), "UnloadDLL");
  assert(UnloadDll);
  SCOPE_EXIT { UnloadDll(); };

  std::vector<ArcDataFile> ArcDataFiles;
  for (std::string line = Resources->GetIniValue(
           "Files", "File" + std::to_string(ArcDataFiles.size() + 1), "");
       !line.empty();
       line = Resources->GetIniValue(
           "Files", "File" + std::to_string(ArcDataFiles.size() + 1), "")) {
    ArcDataFiles.emplace_back(line);
  }

  std::string LastError;
  std::thread arcExtract([&]() {
    for (auto &file : ArcDataFiles) {
      bool checked = file.Component > 0 && file.Component < TInstallerInfo::languagePack
          ? TInstallerInfo[]
      if (!std::filesystem::exists(file.FileName)) {
        LastError = gupta::format("% doesn't exist", file.FileName);
        return;
      }
      if (auto ret = FreeArcExtract(cb, "x", file.FileName.c_str(), nullptr);
          ret) {
        LastError =
            gupta::format("% failed to retrieve size, FREEARC_CODE: %(%)",
                          file.FileName, ret, LastArcErrorMsg);
      } else {
        file.PackedSize = LastArchiveSize;
        debug("%: unpack size = %", file.FileName, file.PackedSize);
      }
    }

    for (auto &file : ArcDataFiles) {
      char *c[20];
      int i;
      for (i = 0; i < 20; i++) {
        file.extractionCmd[i].push_back(0);
        c[i] = file.extractionCmd[i].data();
      }
      if ((i = FreeArcExtract(cb, c[0], c[1], c[2], c[3], c[4], c[5], c[6],
                              c[7], c[8], c[9], c[10], c[11], c[12], c[13],
                              c[14], c[15], c[16], c[17], c[18], c[19],
                              nullptr))) {

        LastError = "FreeArcExtract Failed with error - " + std::to_string(i);
        break;
      }
    }
  });
  arcExtract.join();
  if (LastError.empty())
    throw std::runtime_error{LastError};
  debug("done installation");
} catch (std::exception &e) {
  emit installationFailed(e.what());
}

void TComponent::setChecked(bool s) {
  qDebug(__func__);
  checked = s;
}

#include "tinstallerinfo.hpp"
#include "TResources.hpp"
#include "debug.h"
#include "shortcut.hpp"

#include <QColor>
#include <QDebug>
#include <QProcess>
#include <algorithm>
#include <filesystem>

#include <chrono>
#include <gupta/cleanup.hpp>
#include <gupta/dll.hpp>
#include <infoware/infoware.hpp>

#include "WinReg/WinReg.hpp"
#include <functional>
#include <mutex>
#include <thread>

TInstallerInfo::ResourcePtr TInstallerInfo::Resources;
std::filesystem::space_info TInstallerInfo::m_driveInfo;
QString TInstallerInfo::m_destinationFolder;
QList<QObject *> TInstallerInfo::componentsPack, TInstallerInfo::languagePack,
    TInstallerInfo::redestribPack;
QColor TInstallerInfo::themeColor_;
std::mutex unarcMutex;

TComponent *TInstallerInfo::desktopShortcut, *TInstallerInfo::startMenuShortcut;
QString TInstallerInfo::websiteUrl_, TInstallerInfo::facebookUrl_,
    TInstallerInfo::threadUrl_;
bool TInstallerInfo::terminateInstallation_ = false;

#include <ShlObj.h>
std::filesystem::path desktopDirectory() {
  PWCHAR dpath = NULL;
  HRESULT hr;
  SHOW(hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &dpath));
  if (!SUCCEEDED(hr))
    return "";
  std::filesystem::path res(dpath);
  CoTaskMemFree(dpath);
  return res;
}

std::filesystem::path startMenuDirectory() {
  PWCHAR dpath = NULL;
  HRESULT hr;
  SHOW(hr = SHGetKnownFolderPath(FOLDERID_CommonPrograms, 0, NULL, &dpath));
  if (!SUCCEEDED(hr))
    return "";
  std::filesystem::path res(dpath);
  CoTaskMemFree(dpath);
  return res;
}

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

TRedistributable *RedistribFromIni(std::string str, QObject *parent) {
  auto c = new TRedistributable(parent);
  str.insert(0, ":");
  c->name = LineSection(str, ":").c_str();
  c->checked = StrToBool(LineSection(str, "Checked:"));
  c->Cmd = LineSection(str, "Cmd:");
  return c;
}

TInstallerInfo::TInstallerInfo(QObject *parent) : QObject(parent) {}

TInstallerInfo::~TInstallerInfo() {
  if (installerState_ != TInstallerStates::InstallationNeverStarted)
    installerThread_.join();
}

void TInstallerInfo::setResources(TInstallerInfo::ResourcePtr p) {
  if (!(Resources = std::move(p)))
    return;

  QString pfDir = qgetenv("ProgramFiles").toStdString().c_str();
  setDestinationFolderImpl(
      pfDir + '\\' + Resources->GetIniValue("Setup", "AppName", "").c_str());

  themeColor_ = QColor(Resources->GetIniValue("Setup", "Color", "").c_str());

  auto getUrl = [](auto type) {
    auto s = QString::fromStdString(Resources->GetIniValue("Url", type, ""));
    if (!s.startsWith("https://", Qt::CaseInsensitive))
      s.insert(0, "https://");
    return s;
  };

  websiteUrl_ = getUrl("Website");
  threadUrl_ = getUrl("Thread");
  facebookUrl_ = getUrl("Facebook");

  SHOW(websiteUrl_);
  SHOW(threadUrl_);
  SHOW(facebookUrl_);

  int i = 1;
  std::string name;
  auto readPack = [&](const char *PackName, auto Func) {
    i = 1;
    while ((name = Resources->GetIniValue(PackName,
                                          "Name" + std::to_string(i++), ""))
               .size()) {
      Func();
    }
  };
  readPack("LanguagePacks",
           [&]() { languagePack.push_back(ComponentFromIni(name, nullptr)); });
  readPack("ComponentPacks", [&]() {
    componentsPack.push_back(ComponentFromIni(name, nullptr));
  });
  readPack("Redistributables",
           [&]() { redestribPack.push_back(RedistribFromIni(name, nullptr)); });
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

TInstallerInfo::TInstallerStates TInstallerInfo::getInstallerState() const {
  return installerState_;
}

void TInstallerInfo::setStatusMessage(const QString &StatusMessage) {
  StatusMessage_ = StatusMessage;
  emit statusMessageChanged();
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
  return themeColor_;
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

Q_INVOKABLE double TInstallerInfo::progress() { return Progress_; }

std::vector<uint8_t> GetFile(std::filesystem::path p) {
  std::vector<uint8_t> b;
  auto f = std::fopen(p.string().c_str(), "r");
  b.resize(std::filesystem::file_size(p));
  std::fread(b.data(), 1, b.size(), f);
  fclose(f);
  return b;
}

QString TInstallerInfo::aboutTxt() {
  auto txt = Resources->GetFile("About.txt");
  //  auto txt =
  //      GetFile("E:/Cpp/Projects/Gui/installer/build/Deploy/Resources/About.txt");

  auto about_txt =
      QString::fromLocal8Bit(reinterpret_cast<char *>(txt.data()), txt.size());
  SHOW(about_txt);
  return about_txt;
}

using Number = int;
using cbtype = int __stdcall(char *what, Number int1, Number int2, char *str);

int BytesPerSec;
std::string ArcErrorMsg;
TInstallerInfo *Installer;
intmax_t TotalFiles, OrigSize, CompSize, ArcError, MaxArcWriteSize,
    LastArchivesWriteSize, CurrentArchiveWriteSize;

int __stdcall freearcinfo_callback(char *what, Number int1, Number int2,
                                   char *str) {
  if (!std::strcmp("total_files", what)) {
    TotalFiles = int1;
  } else if (!std::strcmp("origsize", what)) {
    OrigSize = (intmax_t)int1 << 20;
  } else if (!std::strcmp("compsize", what)) {
    CompSize = (intmax_t)int1 << 20;
  } else if (!std::strcmp("error", what)) {
    ArcErrorMsg = str;
    ArcError = int1;
    ExitThread(-1);
  }
  debug("%:unhandled what - %", __func__, what);
  if (TInstallerInfo::isTerminateInstallation()) {
    debug("quitting installation");
    return -127;
  }
  while (Installer->getInstallerState() ==
         TInstallerInfo::TInstallerStates::InstallationPaused) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }
  return 1;
}

int __stdcall cb(char *what, Number int1, Number int2, char *str) {
  unarcMutex.lock();
  SCOPE_EXIT { unarcMutex.unlock(); };
  if (!std::strcmp("write", what)) {
    CurrentArchiveWriteSize = (uint64_t)int1 << 20;
    Installer->setProgress((LastArchivesWriteSize + CurrentArchiveWriteSize) *
                           100.0 / MaxArcWriteSize);
    return 0;
  } else if (!std::strcmp("filename", what)) {
    Installer->setStatusMessage(QString("Unpacking %1").arg(str));
    return 0;
  } else if (!std::strcmp("error", what)) {
    ArcErrorMsg = str;
    ArcError = int1;
    unarcMutex.unlock();
    ExitThread(-1);
    return 0;
  }
  debug("%:unhandled what - %", __func__, what);
  if (TInstallerInfo::isTerminateInstallation()) {
    debug("quitting installation");
    return -127;
  }
  while (Installer->getInstallerState() ==
         TInstallerInfo::TInstallerStates::InstallationPaused) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
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

QString TInstallerInfo::timeToStr(long long s) {
  /*
  QString res;
  const char *r[] = {"second(s)", "minute(s)", "hour(s)"};
  for (int i = 2; i >= 0; i--) {
    if (c > std::pow(60, i)) {
      res += QString::number((int)(c / std::pow(60, i))) + r[i] + ' ';
      c /= std::pow(60, i);
    }
  }
  res.remove(res.size() - 1, 1);
  return res;
  */
  if (s <= 0)
    return "0s";
  QString res;
  res = QString::number(s % 60) + 's';
  s /= 60;
  if (s)
    res.insert(0, QString::number(s % 60) + "m ");
  s /= 60;
  if (s)
    res.insert(0, QString::number(s) + "h ");
  return res;
}

void TInstallerInfo::pauseInstallation() {
  if (installerState_ == TInstallerStates::InstallationRunning)
    installerState_ = TInstallerStates::InstallationPaused;
}

void TInstallerInfo::resumeInstallation() {
  if (installerState_ == TInstallerStates::InstallationPaused)
    installerState_ = TInstallerStates::InstallationRunning;
}

int TInstallerInfo::bytesPerSec() { return BytesPerSec; }

QString TInstallerInfo::destinationFolder() { return m_destinationFolder; }

decltype(std::chrono::high_resolution_clock::now()) InstallationStartTime;
auto now() { return std::chrono::high_resolution_clock::now(); }

inline void TInstallerInfo::setProgress(double progress) {
  SHOW(progress);
  Progress_ = progress;

  auto currentTime = now();
  auto elapsed = std::chrono::duration<double, std::milli>(
                     currentTime - InstallationStartTime)
                     .count();
  elapsed /= 1000;
  totalTime_ = elapsed * 100.0 / progress;
  remainingTime_ = totalTime_ - elapsed;
  BytesPerSec =
      double(CurrentArchiveWriteSize + LastArchivesWriteSize) / elapsed;
  SHOW(BytesPerSec);
  emit progressChanged();
}

struct ArcDataFile {
  std::string ExtractionCmd[20];
  int64_t OrigSize, CompSize;
  TComponent *Component;
  std::string FileName;
  bool Install;
  ArcDataFile(std::string line) {
    int i = 0;
    line.insert(0, ":");
    auto insertCmd = [&](auto... c) {
      assert(i + sizeof...(c) < 20);
      ((ExtractionCmd[i++] = c), ...);
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

    if (auto cfg =
            TInstallerInfo::expandConstant("{tmp}\\arc.ini").toStdString();
        std::filesystem::exists(cfg)) {
      insertCmd("-cfg" + cfg);
    }

    insertCmd("-o+");

    insertCmd("--", FileName = TInstallerInfo::expandConstant(
                                   LineSection(line, ":").c_str())
                                   .toStdString());

    auto CompStr = LineSection(line, "Comp:", "");
    if (CompStr.empty()) {
      Component = nullptr;
    } else {
      std::for_each(CompStr.begin(), CompStr.end(),
                    [](auto &c) { c = tolower(c); });
      auto findComp = [&](const QObject *obj) {
        auto comp = dynamic_cast<const TComponent *>(obj);
        assert(comp && "obj is not a TComponent");
        return comp->name.toLower() == CompStr.c_str();
      };
      auto e = std::find_if(TInstallerInfo::languagePack.begin(),
                            TInstallerInfo::languagePack.end(), findComp);
      if (e != TInstallerInfo::languagePack.end()) {
        Component = dynamic_cast<TComponent *>(*e);
      } else if ((e = std::find_if(TInstallerInfo::componentsPack.begin(),
                                   TInstallerInfo::componentsPack.end(),
                                   findComp)) !=
                 TInstallerInfo::componentsPack.end()) {
        Component = dynamic_cast<TComponent *>(*e);
      } else {
        Component = nullptr;
      }
    }
    Install = !Component || Component->checked;
    if (Component)
      debug("% belongs to %(checked = %)", FileName, Component->name,
            Component->checked);
    debug("install = %", Install);
  }
};

#include <powrprof.h>
int execute(std::string cmd) {
  STARTUPINFOA info = {sizeof(info)};
  PROCESS_INFORMATION processInfo;
  SHOW(cmd);
  int r = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW,
                         NULL, NULL, &info, &processInfo);
  if (r) {
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  } else {
    debug() << "failed to execute " << cmd;
  }
  return r;
}

void TInstallerInfo::startInstallationImpl() try {

  installerRunning_ = true;
  InstallationStartTime = now();
  installerState_ = TInstallerStates::InstallationRunning;

  SCOPE_EXIT {
    installerRunning_ = false;
    if (hibernatePCAfterInstallation_)
      SHOW(SetSuspendState(true, false, false));
  };
  SCOPE_SUCCESS { installerState_ = TInstallerStates::InstallationFinished; };
  SCOPE_FAILURE {
    installerState_ = TInstallerStates::InstallationFailed;
  }; // declare before, since extract uninstaller may fail
  // extract it before, so that can be called on failure
  auto uninstaller_path = Resources->extractFile(
      TResources::path(destinationFolder().toStdWString()) /
          "Uninstall\\uninstaller.exe",
      "private\\uninstaller.exe");
  SCOPE_FAILURE { execute("\"" + uninstaller_path.string() + "\" /silent"); };

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
  Installer = this;
  std::thread arcExtract([&]() {
    MaxArcWriteSize = 0;
    for (auto &file : ArcDataFiles) {
      if (!file.Install)
        continue;
      if (!std::filesystem::exists(file.FileName)) {
        LastError = gupta::format("% doesn't exist", file.FileName);
        return;
      }
      if (auto ret = FreeArcExtract(freearcinfo_callback, "l",
                                    file.FileName.c_str(), nullptr);
          ret) {
        LastError =
            gupta::format("% failed to retrieve size, FREEARC_CODE: %(%)",
                          file.FileName, ArcError, ArcErrorMsg);
      } else {
        file.OrigSize = OrigSize;
        file.CompSize = CompSize;
        debug("%: unpack size = %,%", file.FileName, file.OrigSize,
              file.CompSize);
        MaxArcWriteSize += file.OrigSize;
      }
    }

    SHOW(MaxArcWriteSize);
    LastArchivesWriteSize = 0;
    for (auto &file : ArcDataFiles) {
      if (!file.Install)
        continue;
      char *c[20];
      int i;
      for (i = 0; i < 20; i++) {
        file.ExtractionCmd[i].push_back(0);
        c[i] = file.ExtractionCmd[i].data();
        debug("c[%] = %", i, c[i]);
      }
      if ((i = FreeArcExtract(cb, c[0], c[1], c[2], c[3], c[4], c[5], c[6],
                              c[7], c[8], c[9], c[10], c[11], c[12], c[13],
                              c[14], c[15], c[16], c[17], c[18], c[19],
                              nullptr))) {

        LastError = "FreeArcExtract Failed with error - " + std::to_string(i);
        break;
      }
      LastArchivesWriteSize += CurrentArchiveWriteSize;
    }
  });
  arcExtract.join();
  if (ArcErrorMsg.size())
    LastError = "FREEARC: " + ArcErrorMsg;
  if (LastError.size())
    throw std::runtime_error{LastError};
  if (isTerminateInstallation())
    throw std::runtime_error{"User terminated the Installation"};

  for (auto &r : redestribPack) {
    auto rp = dynamic_cast<TRedistributable *>(r);
    assert(rp && "Ptr is not A TRedistributable");
    debug("running % with \"%\"", rp->name, rp->Cmd.c_str());
    QProcess::execute(expandConstant(rp->Cmd.c_str()));
  }

  debug("creating shortcuts");
  setStatusMessage("Creating Shortcuts");
  int i = 0;
  auto desktopDir = desktopDirectory(), startMenuDir = startMenuDirectory();
  std::string shortCutline;
  while ((shortCutline = Resources->GetIniValue(
              "Shortcuts", "Name" + std::to_string(++i), ""))
             .size()) {
    shortCutline.insert(0, ":");
    std::string name = LineSection(shortCutline, ":") + ".lnk";
    if (name.empty())
      continue;
    std::string srcFile =
        expandConstant(QString(LineSection(shortCutline, "Target:").c_str()))
            .toStdString();
    if (srcFile.empty())
      continue;
    std::string description = LineSection(shortCutline, "Description:");
    if (desktopShortcut->checked &&
        StrToBool(LineSection(shortCutline, "Desktop:", "1"))) {
      SHOW(CreateLink(srcFile.c_str(), (desktopDir / name).string().c_str(),
                      description.c_str()));
    }
    if (startMenuShortcut->checked &&
        StrToBool(LineSection(shortCutline, "StartMenu:", "1"))) {
      SHOW(CreateLink(srcFile.c_str(), (startMenuDir / name).string().c_str(),
                      description.c_str()));
    }
  }

  debug("adding uninstaller reg keys");
  setStatusMessage("Creating uninstaller");
  winreg::RegKey UninstallKey{
      HKEY_LOCAL_MACHINE,
      LR"(Software\Microsoft\Windows\CurrentVersion\Uninstall\)" +
          applicationName().toStdWString()};

  UninstallKey.SetStringValue(L"DisplayName", applicationName().toStdWString());
  UninstallKey.SetStringValue(L"UninstallString",
                              uninstaller_path.wstring().c_str());

  debug("done installation");
  setStatusMessage("Done");
  setProgress(100);
  emit installationCompleted(QString::fromStdString(LastError));

} catch (std::exception &e) {
  debug("error while installation - %", e.what());
  emit installationFailed(e.what());
}

void TComponent::setChecked(bool s) {
  debug("%:%", name, s);
  checked = s;
}

TComponent::~TComponent() { debug("destroyed"); }

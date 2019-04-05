#include "tinstallerinfo.hpp"
#include "TResources.hpp"
#include "Wmi_Object.h"
#include "debug.h"
#include "shortcut.hpp"

#include "WinReg/WinReg.hpp"
#include "buf_io.h"

#include <QColor>
#include <QCryptographicHash>
#include <QDebug>

#include <algorithm>
#include <filesystem>

#include <chrono>
#include <gupta/cleanup.hpp>
#include <gupta/dll.hpp>
#include <gupta/windows.hpp>

#include <ShlObj.h>

using namespace std::chrono_literals;

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

std::vector<std::filesystem::path> SepFiles(std::string s) {
  std::string cs;
  std::vector<std::filesystem::path> r;
  for (auto c : s) {
    if (c == ',') {
      r.push_back(TInstallerInfo::expandConstant(QString::fromStdString(cs)).toStdWString());
      cs.clear();
    } else
      cs += c;
  }
  if (cs.size())
    r.push_back(cs);
  return r;
}

bool SepFilesExists(std::vector<std::filesystem::path> p) {
  for (auto &&cp : p)
    if (!std::filesystem::exists(cp))
      return false;
  return p.size();
}

struct ArcDataFile {
  std::string ExtractionCmd[20];
  int64_t OrigSize, CompSize;
  TComponent *Component;
  std::string FilePath, Password, Md5Hash;
  bool Install() { return !Component || Component->checked; };
  ArcDataFile(std::string line) {
    int i = 0;
    line.insert(0, ":");
    auto insertCmd = [&](auto... c) {
      assert(i + sizeof...(c) < 20);
      ((ExtractionCmd[i++] = c), ...);
    };

    Md5Hash = LineSection(line, "Md5:");

    insertCmd("x");

    if (Password = LineSection(line, "Password:"); !Password.empty()) {
      insertCmd("-p" + Password);
    }

    if (auto tmpDir = LineSection(line, "Tmp:"); !tmpDir.empty()) {
      insertCmd("-w" + TInstallerInfo::expandConstant(tmpDir.c_str()).toStdString());
    } else {
      insertCmd("-w" + TInstallerInfo::expandConstant("{tmp}\\").toStdString());
    }

    if (auto destDir = LineSection(line, "Dest:"); !destDir.empty()) {
      insertCmd("-dp" + TInstallerInfo::expandConstant(destDir.c_str()).toStdString());
    } else {
      insertCmd("-dp" + TInstallerInfo::expandConstant("{app}\\").toStdString());
    }

    if (auto cfg = TInstallerInfo::expandConstant("{tmp}\\arc.ini").toStdString();
        std::filesystem::exists(cfg)) {
      insertCmd("-cfg" + cfg);
    }

    insertCmd("-o+");

    insertCmd("--",
              FilePath =
                  TInstallerInfo::expandConstant(LineSection(line, ":").c_str()).toStdString());

    auto CompStr = LineSection(line, "Comp:", "");
    if (CompStr.empty()) {
      Component = nullptr;
    } else {
      std::for_each(CompStr.begin(), CompStr.end(), [](auto &c) { c = tolower(c); });
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
                                   TInstallerInfo::componentsPack.end(), findComp)) !=
                 TInstallerInfo::componentsPack.end()) {
        Component = dynamic_cast<TComponent *>(*e);
      } else {
        Component = nullptr;
      }
    }
  }
};

// QString OSText = QString::fromStdWString(std::get<std::wstring>(OSInfo.get(L"Caption")));
// QString BuildNumberText =
//    QString::fromStdWString(std::get<std::wstring>(OSInfo.get(L"BuildNumber"))) + '.' +
//    QString::number(/*std::get<uint64_t>(OSInfo.get(L"ServicePackMajorVersion"))*/ 10);

TInstallerInfo::ResourcePtr TInstallerInfo::Resources;
std::filesystem::space_info TInstallerInfo::m_driveInfo;
QString TInstallerInfo::m_destinationFolder;
QList<QObject *> TInstallerInfo::componentsPack, TInstallerInfo::languagePack,
    TInstallerInfo::redestribPack;
QColor TInstallerInfo::themeColor_;
TComponent *TInstallerInfo::desktopShortcut, *TInstallerInfo::startMenuShortcut;
QString TInstallerInfo::websiteUrl_, TInstallerInfo::facebookUrl_, TInstallerInfo::threadUrl_;
bool TInstallerInfo::terminateInstallation_ = false;

TComponent *ComponentFromIni(std::string str, QObject *parent) {
  auto c = new TComponent(parent);
  str.insert(0, ":");
  c->name = LineSection(str, ":").c_str();
  c->checked = StrToBool(LineSection(str, "Checked:")) ||
               SepFilesExists(SepFiles(LineSection(str, "Files:")));
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
  if (installerThread_.joinable())
    installerThread_.join();
}

void TInstallerInfo::setResources(TInstallerInfo::ResourcePtr p) {
  if (!(Resources = std::move(p)))
    return;

  QString pfDir = qgetenv("ProgramFiles").toStdString().c_str();
  setDestinationFolderImpl(pfDir + '\\' + Resources->GetIniValue("Setup", "AppName", "").c_str());

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

  int i = 1;
  std::string name;
  auto readPack = [&](const char *PackName, auto Func) {
    i = 1;
    while ((name = Resources->GetIniValue(PackName, "Name" + std::to_string(i++), "")).size()) {
      Func();
    }
  };
  readPack("LanguagePacks", [&]() { languagePack.push_back(ComponentFromIni(name, nullptr)); });
  readPack("ComponentPacks", [&]() { componentsPack.push_back(ComponentFromIni(name, nullptr)); });
  readPack("Redistributables", [&]() { redestribPack.push_back(RedistribFromIni(name, nullptr)); });

  for (std::string line =
           Resources->GetIniValue("Files", "File" + std::to_string(ArcFiles.size() + 1), "");
       !line.empty();
       line = Resources->GetIniValue("Files", "File" + std::to_string(ArcFiles.size() + 1), "")) {
    ArcFiles.emplace_back(line);
  }
}

void TInstallerInfo::setDestinationFolderImpl(const QString &destinationFolder) {
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

void TInstallerInfo::waitForGuiEvents() {
  if (isTerminateInstallation())
    throw std::runtime_error{"User Terminated the Installation"};
  while (getInstallerState() == TInstallerInfo::TInstallerStates::InstallationPaused)
    std::this_thread::sleep_for(100ms);
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
    d = QString::fromLocal8Bit(reinterpret_cast<const char *>(f.data()), f.size());
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

int TInstallerInfo::diskTotalSpace() { return m_driveInfo.capacity / (1024 * 1024); }

int TInstallerInfo::diskFreeSpace() { return m_driveInfo.free / (1024 * 1024); }

QString TInstallerInfo::info(const QString &s) {
  SHOW(s);
  return Resources->GetIniValue("Setup", s.toStdString(), "").c_str();
}

QString TInstallerInfo::requirement(const QString &s) {
  return QString::fromStdString(Resources->GetIniValue("Requirements", s.toStdString(), ""));
}

QString TInstallerInfo::osText() {
  return QString::fromStdWString(
      std::get<std::wstring>(getWmiProp(L"Win32_OperatingSystem", L"Caption")));
  // return QString::fromStdWString(std::get<std::wstring>(OSInfo.get(L"Caption")));
}

QString TInstallerInfo::buildNumberText() {
  //  auto major = std::get<uint64_t>(getWmiProp(L"Win32_OperatingSystem",
  //  L"ServicePackMajorVersion")); auto minor =
  //  std::get<uint64_t>(getWmiProp(L"Win32_OperatingSystem", L"ServicePackMinorVersion")); return
  //  QString::fromStdWString(std::to_wstring(major) + L'.' + std::to_wstring(minor));

  return QString::fromStdWString(
      std::get<std::wstring>(getWmiProp(L"Win32_OperatingSystem", L"BuildNumber")));
}

QString TInstallerInfo::cpuText() {
  return QString::fromStdWString(std::get<std::wstring>(getWmiProp(L"Win32_Processor", L"Name")));
}
QString TInstallerInfo::cpuFrequencyText() {
  // SHOW();
  // auto s = std::to_wstring(
  //     std::get<std::uint64_t>(getWmiProp(L"Win32_Processor", L"MaxClockSpeed")) / 1000.0);
  return QString::number(std::get<std::uint64_t>(getWmiProp(L"Win32_Processor", L"MaxClockSpeed")) /
                             1000.0,
                         'g', 2) +
         "GHz @ " +
         QString::number(std::get<std::uint64_t>(
             getWmiProp(L"Win32_Processor", L"NumberOfLogicalProcessors"))) +
         " Threads";
}

int TInstallerInfo::ramSize() {
  ULONGLONG r;
  GetPhysicallyInstalledSystemMemory(&r);
  return r / (1024);
}

int TInstallerInfo::ramUsage() {
  MEMORYSTATUSEX status;
  status.dwLength = sizeof status;
  GlobalMemoryStatusEx(&status);
  return ((status.ullTotalPhys - status.ullAvailPhys) * 100) / status.ullTotalPhys;
}

QString TInstallerInfo::gpuText() {
  return QString::fromStdWString(
      std::get<std::wstring>(getWmiProp(L"Win32_VideoController", L"Caption")));
}

static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks) {
  static unsigned long long _previousTotalTicks = 0;
  static unsigned long long _previousIdleTicks = 0;

  unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
  unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

  float ret = 1.0f - ((totalTicksSinceLastTime > 0)
                          ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime
                          : 0);

  _previousTotalTicks = totalTicks;
  _previousIdleTicks = idleTicks;
  return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME &ft) {
  return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in
// between You'll need to call this at regular intervals, since it measures the
// load between the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad() {
  FILETIME idleTime, kernelTime, userTime;
  return GetSystemTimes(&idleTime, &kernelTime, &userTime)
             ? CalculateCPULoad(FileTimeToInt64(idleTime),
                                FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime))
             : -1.0f;
}

int TInstallerInfo::cpuUsage() {
  return std::get<uint64_t>(getWmiProp(L"Win32_Processor", L"LoadPercentage"));
  // return GetCPULoad() * 100;
  // static CpuUsage c;
  // return c.getCpuUsagePercent();
}

QString TInstallerInfo::expandConstant(QString str) {
  str.replace("{app}", destinationFolder());
  str.replace("{src}", QString::fromStdU16String(std::filesystem::current_path().u16string()));
  str.replace("{tmp}", QString::fromStdU16String(Resources->TmpFolder().u16string()));
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

  auto about_txt = QString::fromLocal8Bit(reinterpret_cast<char *>(txt.data()), txt.size());
  SHOW(about_txt);

  return about_txt;
}

int TInstallerInfo::remainingTime() { return remainingTime_; }

int TInstallerInfo::totalTime() { return totalTime_; }

using Number = int;
using cbtype = int __stdcall(char *what, Number int1, Number int2, char *str);

int BytesPerSec;
intmax_t MaxArcWriteSize, LastArchivesWriteSize, CurrentArchiveWriteSize;

void TInstallerInfo::startInstallation() {
  debug("starting installation");
  if (installerRunning_) {
    debug("can't start installation twice");
    return;
  }
  if (installerThread_.joinable())
    installerThread_.join();
  installerThread_ = std::thread(&TInstallerInfo::startInstallationImpl, this);
}

void TInstallerInfo::startVerification() {
  if (installerThread_.joinable())
    installerThread_.join();
  installerThread_ = std::thread(&TInstallerInfo::startVerificationImpl, this);
}

void TInstallerInfo::setDestinationFolder(QString destinationFolder) {
  if (!destinationFolder.endsWith(applicationName())) {
    if (destinationFolder.back() == '/' || destinationFolder.back() == '\\') {
      destinationFolder += applicationName();
    } else {
      destinationFolder += '\\' + applicationName();
    }
  }
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
  auto elapsed =
      std::chrono::duration<double, std::milli>(currentTime - InstallationStartTime).count();
  elapsed /= 1000;
  totalTime_ = elapsed * 100.0 / progress;
  remainingTime_ = totalTime_ - elapsed;
  BytesPerSec = double(CurrentArchiveWriteSize + LastArchivesWriteSize) / elapsed;
  SHOW(BytesPerSec);
  emit progressChanged();
}

std::vector<ArcDataFile> TInstallerInfo::ArcFiles;

#include <powrprof.h>
template <typename... Args> int execute(Args... a) {
  std::string cmd;
  ((cmd += std::string("\"") + a + "\" "), ...);
  cmd.push_back(0);
  STARTUPINFOA info = {sizeof(info)};
  PROCESS_INFORMATION processInfo;
  SHOW(cmd);
  int r = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &info,
                         &processInfo);
  if (r) {
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  } else {
    debug() << "failed to execute " << cmd;
  }
  return r;
}

STARTUPINFOA info_nowait = {sizeof(info_nowait)};
PROCESS_INFORMATION processInfo_nowait = {0};
template <typename... Args> int execute_nowait(Args... a) {
  std::string cmd;
  ((cmd += std::string("\"") + a + "\" "), ...);
  cmd.push_back(0);
  info_nowait = {sizeof(info_nowait)};
  SHOW(cmd);
  int r = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL,
                         &info_nowait, &processInfo_nowait);
  if (r) {
  } else {
    debug() << "failed to execute " << cmd;
  }
  return r;
}

std::string random_key() { return std::to_string(std::rand()); }

enum InfoType : int8_t { None, Completed, Error, WriteSize, FileName };

void TInstallerInfo::startInstallationImpl() try {

  installerRunning_ = true;
  InstallationStartTime = now();
  installerState_ = TInstallerStates::InstallationRunning;

  Progress_ = 0;
  totalTime_ = 0;
  remainingTime_ = 0;
  BytesPerSec = 0;
  emit progressChanged();

  setStatusMessage("Preparing");

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
      TResources::path(destinationFolder().toStdWString()) / "Uninstall\\uninstaller.exe",
      "private\\uninstaller.exe");
  SCOPE_FAILURE {
    setProgress(0);
    setStatusMessage("Cleaning Up");
    execute(uninstaller_path.string(), "/silent");
  };

  auto host_exe = Resources->extractTemporaryFile("private/host.exe").string();
  // auto host_exe = std::string(
  //    R"(E:/Cpp/Projects/Build/build-unarc_x64-boost/build-unarc_x64-boost-Desktop_Qt_5_12_1_MSVC2017_32bit-Default/host.exe)");
  std::string LastError;
  std::thread arcExtract([&]() {
    MaxArcWriteSize = 0;
    gupta::SharedMemory UnarcMem;
    setStatusMessage("Extracting Info");
    if (!UnarcMem.create(random_key(), 1024)) {
      LastError = "Failed to Create Shared Memory";
      return;
    }
    for (auto &file : ArcFiles) {
      if (!file.Install())
        continue;
      if (!std::filesystem::exists(file.FilePath)) {
        LastError = gupta::format("% doesn't exist", file.FilePath);
        return;
      }

      {
        byte_stream s = UnarcMem.data();
        s.write("l");
        if (file.Password.size()) {
          SHOW(file.Password);
          s.write("-p" + file.Password);
        }
        s.write(file.FilePath);
      }

      if (!execute(host_exe, UnarcMem.name())) {
        LastError = "Failed to execute Host";
        return;
      }

      {
        read_stream s = UnarcMem.data();
        while (1) {
          auto what = s.read<std::string>();
          auto int1 = s.read<uint64_t>();
          auto int2 = s.read<uint64_t>();
          SHOW(what);
          if (what == "origsize")
            file.OrigSize = int1;
          else if (what == "compsize")
            file.CompSize = int1;
          else if (what == "error") {
            LastError = gupta::format("Failed in Preparations for %: %", file.FilePath,
                                      s.read<std::string>());
            return;
          } else if (what == "completed") {
            break;
          } else if (what != "total_files") {
            LastError = gupta::format("Failed in Preparations for %: %", file.FilePath,
                                      "Invalid What by host - " + what);
            return;
          }
        }
      }

      MaxArcWriteSize += file.OrigSize;
      SHOW(file.OrigSize);
      SHOW(file.CompSize);
      SHOW(MaxArcWriteSize);
    }

    SHOW(MaxArcWriteSize);
    LastArchivesWriteSize = 0;

    setStatusMessage("Starting Extraction");
    for (auto &file : ArcFiles) {
      if (!file.Install())
        continue;
      int i;
      byte_stream s = UnarcMem.data();
      for (i = 0; i < 20; i++) {
        s.write(file.ExtractionCmd[i]);
      }

      {
        auto r = execute_nowait(host_exe, UnarcMem.name());
        if (!r) {
          LastError = "Failed To Execute host for extraction";
          return;
        }
        SCOPE_EXIT {
          TerminateProcess(processInfo_nowait.hProcess, -1);
          CloseHandle(processInfo_nowait.hProcess);
          CloseHandle(processInfo_nowait.hThread);
        };

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);

        DWORD e = 0;
        while (GetExitCodeProcess(processInfo_nowait.hProcess, &e) && e == STILL_ACTIVE) {
          {
            UnarcMem.lock();
            SCOPE_EXIT { UnarcMem.unlock(); };
            read_stream s = UnarcMem.data();
            auto info = s.read<InfoType>();
            if (info == InfoType::WriteSize) {
              CurrentArchiveWriteSize = s.read<uint64_t>();
            } else if (info == InfoType::Error) {
              LastError =
                  gupta::format("Unpacking Failed for %, %", file.FilePath, s.read<std::string>());
              return;
            } else if (info == InfoType::Completed) {
              break;
            }
            info = s.read<InfoType>();
            if (info == InfoType::FileName) {
              setStatusMessage("Unpacking " + QString::fromStdString(s.read<std::string>()));
            }
            waitForGuiEvents();
          }
          setProgress((LastArchivesWriteSize + CurrentArchiveWriteSize) * 100.0 / MaxArcWriteSize);
          std::this_thread::sleep_for(500ms);
        }
        LastArchivesWriteSize += file.OrigSize;
        debug("host Returned : %", e);
      }
    }
  });
  arcExtract.join();

  if (LastError.size())
    throw std::runtime_error{LastError};
  waitForGuiEvents();

  for (auto &r : redestribPack) {
    auto rp = dynamic_cast<TRedistributable *>(r);
    assert(rp && "Ptr is not A TRedistributable");
    debug("running % with \"%\"", rp->name, rp->Cmd.c_str());
    execute(expandConstant(rp->Cmd.c_str()).toStdString());
  }

  debug("creating shortcuts");
  setStatusMessage("Creating Shortcuts");
  int i = 0;
  auto desktopDir = desktopDirectory(), startMenuDir = startMenuDirectory();
  std::string shortCutline;
  while ((shortCutline = Resources->GetIniValue("Shortcuts", "Name" + std::to_string(++i), ""))
             .size()) {
    shortCutline.insert(0, ":");

    TComponent *Component;
    auto CompStr = LineSection(shortCutline, "Comp:", "");
    if (CompStr.empty()) {
      Component = nullptr;
    } else {
      std::for_each(CompStr.begin(), CompStr.end(), [](auto &c) { c = tolower(c); });
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
                                   TInstallerInfo::componentsPack.end(), findComp)) !=
                 TInstallerInfo::componentsPack.end()) {
        Component = dynamic_cast<TComponent *>(*e);
      } else {
        Component = nullptr;
      }
    }
    auto createThisShortcut = !Component || Component->checked;
    if (!createThisShortcut)
      continue;

    std::string name = LineSection(shortCutline, ":") + ".lnk";
    SHOW(name);
    if (name.empty())
      continue;
    auto srcFile =
        expandConstant(QString(LineSection(shortCutline, "Target:").c_str())).toStdWString();
    if (srcFile.empty())
      continue;
    std::string description = LineSection(shortCutline, "Description:");

    SHOW(description);
    if (desktopShortcut->checked && StrToBool(LineSection(shortCutline, "Desktop:", "1"))) {
      SHOW(CreateLink(srcFile.c_str(), (desktopDir / name).wstring().c_str(),
                      QString::fromStdString(description).toStdWString().c_str(),
                      std::filesystem::path(srcFile).parent_path().wstring().c_str()));
    }
    if (startMenuShortcut->checked && StrToBool(LineSection(shortCutline, "StartMenu:", "1"))) {
      SHOW(CreateLink(srcFile.c_str(), (startMenuDir / name).wstring().c_str(),
                      QString::fromStdString(description).toStdWString().c_str(),
                      std::filesystem::path(srcFile).parent_path().wstring().c_str()));
    }
  }

  debug("adding uninstaller reg keys");
  setStatusMessage("Creating uninstaller");
  winreg::RegKey UninstallKey{HKEY_LOCAL_MACHINE,
                              LR"(Software\Microsoft\Windows\CurrentVersion\Uninstall\)" +
                                  applicationName().toStdWString()};

  UninstallKey.SetStringValue(L"DisplayName", applicationName().toStdWString());
  UninstallKey.SetStringValue(L"UninstallString", uninstaller_path.wstring().c_str());

  debug("done installation");
  setStatusMessage("Completed");
  setProgress(100);

  waitForGuiEvents();

  emit installationCompleted(QString::fromStdString(LastError));

} catch (std::exception &e) {
  debug("error while installation - %", e.what());
  emit installationFailed(e.what());
}

void TInstallerInfo::startVerificationImpl() try {
  InstallationStartTime = now();
  installerState_ = TInstallerStates::InstallationRunning;
  SCOPE_EXIT { installerState_ = TInstallerStates::InstallationNeverStarted; };

  QCryptographicHash Hasher(QCryptographicHash::Md5);
  MaxArcWriteSize = 0;
  LastArchivesWriteSize = 0;

  std::for_each(ArcFiles.begin(), ArcFiles.end(), [&](ArcDataFile &f) {
    if (f.Md5Hash.empty()) {
      throw std::runtime_error{f.FilePath + " is not provided with MD5 hash"};
    }
    if (f.Install()) {
      if (!std::filesystem::exists(f.FilePath)) {
        throw std::runtime_error{f.FilePath + " doesn't exists"};
      }
      MaxArcWriteSize += std::filesystem::file_size(f.FilePath);
    }
  });

  {
    char data[1024 * 512];
    size_t data_sz = 0;
    std::for_each(ArcFiles.begin(), ArcFiles.end(), [&](ArcDataFile &f) {
      auto file_name = std::filesystem::path(f.FilePath).filename();
      setStatusMessage("Verifying " + QString::fromStdWString(file_name.wstring()));
      Hasher.reset();
      FILE *file = std::fopen(f.FilePath.c_str(), "rb");
      if (!file) {
        throw std::runtime_error{"Failed to read " + f.FilePath};
      }
      SCOPE_EXIT { std::fclose(file); };
      CurrentArchiveWriteSize = 0;
      while ((data_sz = std::fread(data, 1, sizeof data, file))) {
        Hasher.addData(data, data_sz);
        CurrentArchiveWriteSize += data_sz;
        setProgress((CurrentArchiveWriteSize + LastArchivesWriteSize) * 100.0 / MaxArcWriteSize);
        waitForGuiEvents();
      }
      LastArchivesWriteSize += CurrentArchiveWriteSize;
      auto resultHash = Hasher.result().toHex().toStdString();
      waitForGuiEvents();
      if (resultHash != f.Md5Hash) {
        throw std::runtime_error{
            "Hash doesn't match for " + file_name.string() +
            gupta::format(", Produced = \"%\" Required = \"%\"", resultHash, f.Md5Hash)};
      }
    });
  }
  waitForGuiEvents();
  setStatusMessage("Verification Completed Successfully");
  emit verificationCompleted("Verification Completed Successfully");
} catch (std::exception &e) {
  setStatusMessage(e.what());
  emit verificationFailed(QString::fromLocal8Bit(e.what()));
}

void TComponent::setChecked(bool s) {
  debug("%:%", name, s);
  checked = s;
}

TComponent::~TComponent() { debug("destroyed"); }

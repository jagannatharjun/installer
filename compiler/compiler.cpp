#include <Windows.h>
#include <concatfiles.hpp>
#include <cstdio>
#include <cstdlib>
#include <gupta/format_io.hpp>
#include <gupta/ini.hpp>
#include <iostream>

int execute(std::string cmd) {
  STARTUPINFO info = {sizeof(info)};
  PROCESS_INFORMATION processInfo;
  SHOW(cmd);
  int r = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &info,
                         &processInfo);
  if (r) {
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  } else {
    std::cerr << "failed to execute " << cmd;
  }
  return r;
}

void EncryptDecrypt(std::vector<uint8_t> &b) {
  const char key[] = {'G', 'U', 'P', 'T', 'A'};
  for (int i = 0; i < b.size(); i++)
    b[i] = b[i] ^ key[i % (sizeof(key) / sizeof(char))];
}

std::vector<uint8_t> ResourcesInBuffer(const gupta::cf_path &ResourcesFolder) {
  auto archive = gupta::concatfiles(ResourcesFolder);
  std::vector<uint8_t> buf;
  gupta::cf_size_type TotalReadCount = 0, ReadCount = 0, BufferSize = 1024;
  buf.resize(BufferSize);
  while ((ReadCount = archive->read(buf.data() + TotalReadCount,
                                    buf.size() - TotalReadCount))) {
    TotalReadCount += ReadCount;
    buf.resize(TotalReadCount + BufferSize);
  }
  buf.resize(TotalReadCount);
  std::cout << "Resources size = " << buf.size() << std::endl;
  EncryptDecrypt(buf);
  return buf;
}

void writeResources(const std::filesystem::path &ResourcesFolder,
                    const std::filesystem::path &outInstallerExe) {
  auto ResBuf = ResourcesInBuffer(ResourcesFolder);
  auto exe = std::fopen(outInstallerExe.string().c_str(), "ab");
  fwrite(ResBuf.data(), 1, ResBuf.size(), exe);
  uint64_t BufferSize = ResBuf.size();
  printf("BufferSize = %lld\n", BufferSize);
  fwrite((const uint8_t *)&BufferSize, 1, sizeof BufferSize, exe);
  fclose(exe);
}

int main() try {
  //#ifdef NDEBUG
  const std::filesystem::path InstallerExePath = INSTALLER_EXE_PATH;
  const std::filesystem::path outInstallerExe = OUT_INSTALLER_EXE;
  const std::filesystem::path ResourcesFolder = RESOURCES_FOLDER;
  //#else
  //  const auto InstallerExePath = "mainwindow.exe";
  //  const auto outInstallerExe = InstallerExePath;
  //  const std::filesystem::path ResourcesFolder =
  //      R"(E:\Cpp\Projects\Gui\installer\build\Deploy\Resources)";
  //#endif

  SHOW(InstallerExePath.string());
  SHOW(outInstallerExe.string());
  SHOW(ResourcesFolder.string());

  if (!std::filesystem::exists(InstallerExePath)) {
    std::cerr << InstallerExePath << " doesn't exists";
    return 1;
  }
  if (!std::filesystem::exists(ResourcesFolder)) {
    std::cerr << ResourcesFolder << " doesn't exists";
    return 2;
  }

  if (InstallerExePath != outInstallerExe) {
    std::filesystem::remove(outInstallerExe);
    std::filesystem::copy(InstallerExePath, outInstallerExe);
  }

  const auto inifile = ResourcesFolder / "Setup.ini";
  gupta::ParsedIni ini;
  {
    auto ini_file_size = std::filesystem::file_size(inifile);
    std::vector<uint8_t> ini_buffer;
    ini_buffer.resize(ini_file_size);
    auto f = std::fopen(inifile.string().c_str(), "rb");
    if (!f) {
      std::cerr << "Failed to open - " << inifile;
      return 5;
    }
    fread(ini_buffer.data(), 1, ini_buffer.size(), f);
    fclose(f);
    ini = gupta::ParseIni(ini_buffer);
  }
  auto getIniStr = [&](std::string section, std::string key) -> std::string {
    auto f = ini.find(section);
    if (f == ini.end())
      return "";
    auto k = f->second.find(key);
    if (k == f->second.end())
      return "";
    return k->second;
  };
  auto getShorcutFiles = [&]() {
    int i = 1;
    std::string t;
    for (std::string kval;
         !(kval = getIniStr("Shortcuts", "Name" + std::to_string(i++)))
              .empty();) {
      t += kval.substr(0, kval.find_first_of(';')) + '\t';
    }
    return t;
  };

  auto app_name = ini["Setup"]["AppName"];

  {
    using namespace std::filesystem;
    path rcedit_exe = RCEDIT_EXE;
    if (!exists(rcedit_exe)) {
      std::cerr << rcedit_exe << " doesn't exit";
      return 1;
    }

    auto quoted = [](std::string str) { return "\"" + str + "\""; };

    auto makeRcEditCmd = [&](auto... args) {
      std::string cmd = "\"" + rcedit_exe.string() + "\" ";
      ((cmd += quoted(args) + " "), ...);
      return cmd;
    };

    auto changeIcon = [&](const path &exe, const path &icon_file) {
      if (!exists(exe)) {
        std::cout << exe << "- doesn't exists";
        exit(1);
      } else if (!exists(icon_file)) {
        std::cout << icon_file << " - doesn't exists";
        exit(2);
      }
      auto change_icon_cmd = "\"" + rcedit_exe.string() + "\" \"" +
                             exe.string() + "\" --set-icon \"" +
                             icon_file.string() + "\"";
      execute(change_icon_cmd);
    };

    auto icon_file = ResourcesFolder / "icon.ico";
    changeIcon(outInstallerExe, icon_file);

    auto uninstaller_res = UNINSTALLER_EXE;
    auto uninstaller = ResourcesFolder / ("private\\uninstaller.exe");
    SHOW(uninstaller);
    if (!exists(uninstaller.parent_path()))
      create_directories(uninstaller.parent_path());
    else if (exists(uninstaller))
      remove(uninstaller);

    copy(uninstaller_res, uninstaller);
    changeIcon(uninstaller, icon_file);

    execute(makeRcEditCmd(uninstaller.string(), "--set-resource-string", "11",
                          app_name));
    execute(makeRcEditCmd(uninstaller.string(), "--set-resource-string", "12",
                          getShorcutFiles().c_str()));

#ifdef NDEBUG
    execute(makeRcEditCmd(outInstallerExe.string(),
                          "--set-requested-execution-level",
                          "requireAdministrator"));
    execute(makeRcEditCmd(uninstaller.string(),
                          "--set-requested-execution-level",
                          "requireAdministrator"));
#endif
  }

  writeResources(ResourcesFolder, outInstallerExe);

  std::cout << "done";
} catch (std::exception &e) {
  debug("caught(%): %", typeid(e).name(), e.what());
}

#include "TResources.hpp"
#include "debug.h"
#include <Windows.h>
#include <cctype>
#include <concatfiles.hpp>
#include <cstdio>
#include <gupta/cleanup.hpp>
#include <gupta/ini.hpp>
#include <gupta/windows.hpp>
#include <string_view>

template class std::shared_ptr<TResources>;
template class std::map<std::string, gupta::ParsedIni>;

auto myTempDir() {
  TResources::path t;
  while (std::filesystem::exists(t = std::filesystem::temp_directory_path() /
                                     ("installer-" + std::to_string(rand()))))
    ;
  std::filesystem::create_directory(t);
  return t;
}

auto getCurrentExecutable() {
  TResources::path e;
  wchar_t f[1024] = {};
  SHOW(GetModuleFileNameW(NULL, f, 1024));
  e = f;
  SHOW(e.string());
  return e;
}

void EncryptDecrypt(std::vector<uint8_t> &b) {
  const char key[] = "{'G', 'U', 'P', 'T', 'A'}asdgqwor@#$@#%@#%@$%tu124323546134521124555345";
  for (int i = 0; i < b.size(); i++)
    b[i] = b[i] ^ key[i % ((sizeof(key) / sizeof(char)) - 1)];
}

#include <thread>
using namespace std::chrono_literals;

TResources::TResources(std::filesystem::path Source)
    : Source_{std::move(Source)}, TmpFolder_{myTempDir()} {
  //#ifndef NDEBUG
  //  return;
  //#endif
  SHOW(TmpFolder_);

  gupta::WinHandle ExeHandle =
      CreateFileW(getCurrentExecutable().wstring().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (ExeHandle == INVALID_HANDLE_VALUE) {
    throw std::runtime_error{"Failed to Open Exe: " + gupta::GetLastErrorAsString()};
  }

  if (SetFilePointer(ExeHandle, -8, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
    throw std::runtime_error{"Seek Failed for Exe : " + gupta::GetLastErrorAsString()};
  }

  uint64_t BufSize = 0;
  DWORD bytesRead = 0;
  if (!ReadFile(ExeHandle, &BufSize, 8, &bytesRead, NULL)) {
    throw std::runtime_error{"Reading Buffer Size Failed : " + gupta::GetLastErrorAsString()};
  }

  if (SetFilePointer(ExeHandle, -(BufSize + 8), NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
    throw std::runtime_error{"Seek Failed for Exe : " + gupta::GetLastErrorAsString()};
  }

  ExeResourceBuf_.resize(BufSize);

  if (!ReadFile(ExeHandle, ExeResourceBuf_.data(), BufSize, &bytesRead, NULL)) {
    throw std::runtime_error{"Reading Buffer Failed : " + gupta::GetLastErrorAsString()};
  }

  //  FILE *exe = NULL;
  //  for (int i = 0; i < 5 && !(exe = std::fopen(getCurrentExecutable().string().c_str(), "rb"));
  //  i++)
  //    std::this_thread::sleep_for(100ms);
  //  if (!exe) {
  //    perror("failed to open exe for resources");
  //    throw std::runtime_error{"failed to open exe for resources"};
  //  }
  //  fseek(exe, -sizeof(uint64_t), SEEK_END);
  //  uint64_t buf_size = 0;
  //  SHOW(fread(&buf_size, sizeof buf_size, 1, exe));
  //  SHOW(buf_size);
  //  ExeResourceBuf_.resize(buf_size);
  //  fseek(exe, -(buf_size + sizeof(uint64_t)), SEEK_END);
  //  SHOW(fread(ExeResourceBuf_.data(), 1, buf_size, exe));
  // fclose(exe);
  EncryptDecrypt(ExeResourceBuf_);
  auto archive = gupta::openConcatFileStream(ExeResourceBuf_.data(), ExeResourceBuf_.size());
  for (auto f = archive->next_file(); f; f = archive->next_file()) {
    SHOW(f->path().string());
    Files_.emplace_back(std::move(f));
  }
}

void tresources_dir_remove(const std::filesystem::path &Dir) {
  std::error_code ec;
  for (const auto &f : std::filesystem::directory_iterator(Dir)) {
    if (std::filesystem::is_directory(f))
      tresources_dir_remove(f);
    int i = ::remove(f.path().string().c_str());
    debug("%: %", f.path(), i);
  }
}

TResources::~TResources() { tresources_dir_remove(TmpFolder_); }

TResources::path TResources::extractFile(TResources::path DestFile,
                                         const TResources::path &SrcFile) {
  SHOW(DestFile);
  SHOW(SrcFile);
  if (!std::filesystem::exists(DestFile)) {
    std::filesystem::create_directories(DestFile.parent_path());
  }
  if (std::filesystem::is_directory(DestFile))
    DestFile /= SrcFile.filename();
  buffer_t file_buf = GetFile(SrcFile);
  std::FILE *f = std::fopen(DestFile.string().c_str(), "wb");
  if (!f)
    throw std::runtime_error{"failed to write " + DestFile.string()};
  SHOW(file_buf.size());
  std::fwrite(file_buf.data(), 1, file_buf.size(), f);
  fclose(f);
  return DestFile;
}

TResources::path TResources::extractTemporaryFile(TResources::path File) {
  std::vector<uint8_t> FileBuf_;
  GetFile(File, FileBuf_);
  if (FileBuf_.size() == 0)
    throw std::runtime_error{"Failed to Read \"" + File.string() + "\""};
  File = TmpFolder_ / File.filename();
  std::FILE *tmpFile = std::fopen(File.string().c_str(), "wb");
  if (!tmpFile)
    throw std::runtime_error{"Failed to write tmp File \"" + File.string() + "\""};
  std::fwrite(FileBuf_.data(), 1, FileBuf_.size(), tmpFile);
  fclose(tmpFile);
  return File;
}

void TResources::extractTemporaryFiles(const char *WildCard) {
  auto files = GetFiles(WildCard);
  for (auto f : files)
    extractTemporaryFile(f->path());
}

TResources::buffer_t TResources::GetFile(std::filesystem::path File) {
  buffer_t buf;
  return GetFile(std::move(File), buf);
}

TResources::buffer_t TResources::GetFileText(TResources::path File) {
  buffer_t buf;
  return GetFileText(std::move(File), buf);
}

TResources::buffer_t &TResources::GetFile(std::filesystem::path File, TResources::buffer_t &buf) {
  return GetFile(std::move(File), buf, "rb");
}

TResources::buffer_t &TResources::GetFileText(TResources::path File, TResources::buffer_t &buf) {
  return GetFile(std::move(File), buf, "r");
}

TResources::string TResources::GetIniValue(const TResources::path &IniFile,
                                           const TResources::string &Section,
                                           const TResources::string &key, const string &Default) {
  if (Inis_.find(IniFile.string()) == Inis_.end()) {
    auto IniBuf = GetFile((IniFile));
    SHOW(IniFile.string());
    Inis_[IniFile.string()] = gupta::ParseIni(IniBuf);
  }
  auto &f = Inis_[IniFile.string()];
  if (auto siter = f.find(Section); siter != f.end()) {
    if (auto kiter = siter->second.find(key); kiter != siter->second.end())
      return kiter->second;
  }
  return Default;
}

TResources::string TResources::GetIniValue(const TResources::string &Section,
                                           const TResources::string &key,
                                           const TResources::string &Default) {
  return GetIniValue(MainIni_, Section, key, Default);
}

TResources::string TResources::MainIni() const { return MainIni_; }

void TResources::setMainIni(const string &MainIni) { MainIni_ = MainIni; }

TResources::path TResources::TmpFolder() const { return TmpFolder_; }

TResources::buffer_t &TResources::GetFile(TResources::path File, TResources::buffer_t &buf,
                                          const char * /*FileOpenMode*/) {
  //#ifndef NDEBUG
  //  static path resource_path = R"(E:\Cpp\Projects\Gui\installer\build\Deploy\Resources)";
  //  auto f_p = resource_path / File;
  //  auto f_sz = std::filesystem::file_size(f_p);
  //  if (FILE *f = std::fopen(f_p.string().c_str(), "rb"); f) {
  //    buf.resize(f_sz);
  //    fread(buf.data(), 1, buf.size(), f);
  //  }
  //  return buf;
  //#endif

  gupta::cf_basicfile *requiredfile = nullptr;
  std::error_code ec;
  auto fpstr = File.string();
  std::for_each(fpstr.begin(), fpstr.end(), [](auto &c) {
    c = ::tolower(c);
    if (c == '/')
      c = '\\';
  });
  SHOW(fpstr);
  for (auto &f : Files_) {
    auto p = f->path().string();
    std::for_each(p.begin(), p.end(), [](auto &c) {
      c = ::tolower(c);
      if (c == '/')
        c = '\\';
    });
    if (p == fpstr) {
      debug("found %", p);
      requiredfile = f.get();
      break;
    }
  }
  if (!requiredfile)
    throw std::invalid_argument{"can't find " + File.string()};
  buf.resize(requiredfile->size());

  auto seekablefile = dynamic_cast<gupta::cf_seekablefile *>(requiredfile);
  assert(seekablefile);
  seekablefile->seek(0, SEEK_SET);

  auto read_sz = requiredfile->read(buf.data(), requiredfile->size());
  assert(read_sz == requiredfile->size());
  return buf;
}

bool wildcardmatch(const char *s, const char *wildcard) {
  while (*s && *wildcard) {
    if (*wildcard == '*') {
      ++wildcard;
      while (*s != *wildcard)
        s++;
    } else if (*wildcard == '?' || *wildcard == *s) {
      ++wildcard, ++s;
    } else
      break;
  }
  return *s == *wildcard;
}

std::vector<gupta::cf_basicfile *> TResources::GetFiles(std::string WildCard) {
  std::vector<gupta::cf_basicfile *> result;
  std::for_each(WildCard.begin(), WildCard.end(), [](auto &c) { c = c == '/' ? '\\' : c; });
  for (auto &f : Files_)
    if (wildcardmatch(f->path().string().c_str(), WildCard.c_str())) {
      debug("% matched with %", WildCard, f->path().string());
      result.push_back(f.get());
    }
  return result;
}

#include "TResources.hpp"
#include "debug.h"
#include <Windows.h>
#include <cctype>
#include <concatfiles.hpp>
#include <cstdio>
#include <gupta/cleanup.hpp>
#include <gupta/ini.hpp>
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
  return e = f;
}

TResources::TResources(std::filesystem::path Source)
    : Source_{std::move(Source)}, TmpFolder_{myTempDir()} {
  FILE *exe = std::fopen(getCurrentExecutable().string().c_str(), "rb");
  if (!exe)
    throw std::runtime_error{"failed to open exe for resources"};
  fseek(exe, -sizeof(uint64_t), SEEK_END);
  uint64_t buf_size = 0;
  SHOW(fread(&buf_size, sizeof buf_size, 1, exe));
  SHOW(buf_size);
  ExeResourceBuf_.resize(buf_size);
  fseek(exe, buf_size + sizeof uint64_t, SEEK_END);
  SHOW(fread(ExeResourceBuf_.data(), 1, buf_size, exe));
  auto archive = gupta::openConcatFileStream(ExeResourceBuf_.data(), buf_size);
  for (auto f = archive->next_file(); f; f = archive->next_file()) {
    SHOW(f->path());
    Files_.emplace_back(std::move(f));
  }
}

TResources::~TResources() {
  std::error_code ec;
  std::filesystem::remove_all(TmpFolder_, ec);
  SHOW(ec.message());
}

TResources::path TResources::extractTemporaryFile(TResources::path File) {
  std::vector<uint8_t> FileBuf_;
  GetFile(File, FileBuf_);
  if (FileBuf_.size() == 0)
    throw std::runtime_error{"Failed to Read \"" + File.string() + "\""};
  File = TmpFolder_ / File.filename();
  std::FILE *tmpFile = std::fopen(File.string().c_str(), "wb");
  if (!tmpFile)
    throw std::runtime_error{"Failed to write tmp File \"" + File.string() +
                             "\""};
  std::fwrite(FileBuf_.data(), 1, FileBuf_.size(), tmpFile);
  fclose(tmpFile);
  return File;
}

TResources::buffer_t TResources::GetFile(std::filesystem::path File) {
  buffer_t buf;
  return GetFile(std::move(File), buf);
}

TResources::buffer_t TResources::GetFileText(TResources::path File) {
  buffer_t buf;
  return GetFileText(std::move(File), buf);
}

TResources::buffer_t &TResources::GetFile(std::filesystem::path File,
                                          TResources::buffer_t &buf) {
  return GetFile(std::move(File), buf, "rb");
}

TResources::buffer_t &TResources::GetFileText(TResources::path File,
                                              TResources::buffer_t &buf) {
  return GetFile(std::move(File), buf, "r");
}

TResources::string TResources::GetIniValue(const TResources::path &IniFile,
                                           const TResources::string &Section,
                                           const TResources::string &key,
                                           const string &Default) {
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

TResources::buffer_t &TResources::GetFile(TResources::path File,
                                          TResources::buffer_t &buf,
                                          const char * /*FileOpenMode*/) {
  gupta::cf_basicfile *requiredfile = nullptr;
  for (auto &f : Files_)
    if (f->path() == File) {
      requiredfile = f.get();
      break;
    }
  if (!requiredfile)
    throw std::invalid_argument{"can't find " + File.string()};
  buf.resize(requiredfile->size());
  assert(requiredfile->read(buf.data(), requiredfile->size()) ==
         requiredfile->size());
  return buf;
}

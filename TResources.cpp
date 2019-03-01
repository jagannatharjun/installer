#include "TResources.hpp"
#include "debug.h"
#include <cctype>
#include <cstdio>
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

TResources::TResources(std::filesystem::path Source)
    : Source_{std::move(Source)}, TmpFolder_{myTempDir()} {}

TResources::~TResources() {
  std::error_code ec;
  std::filesystem::remove_all(TmpFolder_, ec);
  SHOW(ec.message());
}

TResources::path TResources::extractTemporaryFile(TResources::path File) {
  FileBuf_.resize(0);
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
                                          const char *FileOpenMode) {
  File = Source_ / File;
  std::FILE *f = std::fopen(File.string().c_str(), FileOpenMode);
  if (f) {
    buf.resize(std::filesystem::file_size(File));
    fread(&buf[0], 1, buf.size(), f);
    fclose(f);
  } else
    buf.resize(0);
  return buf;
}

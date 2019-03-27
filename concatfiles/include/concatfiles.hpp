// concatfiles.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <filesystem>
#include <memory>

namespace gupta {

using cf_path = std::filesystem::path;
using cf_size_type = int64_t;

class cf_outputstream {
public:
  using size_type = cf_size_type;
  virtual size_type read(uint8_t *buffer, size_type buffer_size) = 0;
};

class cf_basicfile {
public:
  virtual cf_path path() = 0;
  virtual cf_size_type size() = 0;
  virtual cf_size_type read(uint8_t *buffer, cf_size_type buffer_size) = 0;
};

class cf_seekablefile : public cf_basicfile {
public:
  // implements c-style seek function
  virtual int seek(cf_size_type offset, int origin) = 0;
  virtual cf_size_type tell() = 0;
};

class cf_archive {
public:
  using size_type = int64_t;
  virtual std::unique_ptr<cf_basicfile> next_file() = 0;
};

std::unique_ptr<cf_outputstream> concatfiles(cf_path BaseDir);
std::unique_ptr<cf_archive> openConcatFileStream(const uint8_t *buf,
                                                 cf_size_type buffer_size);

}; // namespace gupta

// TODO: Reference additional headers your program requires here.

#include <concatfiles.hpp>
#include <cstdio>

std::vector<uint8_t> ResourcesInBuffer(gupta::cf_path ResourcesFolder) {
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
  return buf;
}

int main() {
  const auto InstallerExePath = "installer_res.exe";
	const auto outInstallerExe  = "installer.exe";
  const auto ResourcesFolder = R"(Resources)";

  if (!std::filesystem::exists(InstallerExePath)) {
    printf("%s - doesn't exists", InstallerExePath);
    return 1;
  }
  if (!std::filesystem::exists(ResourcesFolder)) {
    printf("%s - doesn't exists", ResourcesFolder);
    return 2;
  }

  auto ResBuf = ResourcesInBuffer(ResourcesFolder);

	std::filesystem::remove(outInstallerExe);
	std::filesystem::copy(InstallerExePath, outInstallerExe);

  auto exe = std::fopen(outInstallerExe, "ab");
  fwrite(ResBuf.data(), 1, ResBuf.size(), exe);
  uint64_t BufferSize = ResBuf.size();
  printf("BufferSize = %lld\n", BufferSize);
  fwrite((const uint8_t *)&BufferSize, 1, sizeof BufferSize, exe);
  fclose(exe);

}
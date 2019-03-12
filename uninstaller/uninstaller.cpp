#include <Windows.h>
#include <string>

std::string getexename() {
  char f[1024] = {};
  GetModuleFileNameA(nullptr, f, sizeof f);
  return f;
}

std::string application_name() {
  std::string e = getexename();
  e = e.substr(e.find_last_of('\\') + 1);
  if (e.rfind(".exe") == e.size() - 4)
    e.resize(e.size() - 4);
  return e;
}

std::string getappfolder() {
  auto e = getexename();
  e = e.substr(0, e.find_last_of('\\'));
  return e;
}

#ifdef NDEBUG
#define MAIN                                                                   \
  __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,              \
                    LPSTR lpCmdLine, int nShowCmd)
#else
#define MAIN main()
#endif

int MAIN {
  auto app = application_name();
  auto msgBoxRet = MessageBoxA(
      NULL,
      ("Do you really want to uninstall - " + app + "?").c_str(),
      "Uninstaller", MB_YESNO);
  if (msgBoxRet != IDYES)
    return 0;
}
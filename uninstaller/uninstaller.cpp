#include "../WinReg/WinReg.hpp"
#include <Windows.h>
#include <filesystem>
#include <string>
#include <strsafe.h>

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey) {
  LPTSTR lpEnd;
  LONG lResult;
  DWORD dwSize;
  TCHAR szName[MAX_PATH];
  HKEY hKey;
  FILETIME ftWrite;

  // First, see if we can delete the key without having
  // to recurse.

  lResult = RegDeleteKey(hKeyRoot, lpSubKey);

  if (lResult == ERROR_SUCCESS)
    return TRUE;

  lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

  if (lResult != ERROR_SUCCESS) {
    if (lResult == ERROR_FILE_NOT_FOUND) {
      printf("Key not found.\n");
      return TRUE;
    } else {
      printf("Error opening key.\n");
      return FALSE;
    }
  }

  // Check for an ending slash and add one if it is missing.

  lpEnd = lpSubKey + lstrlen(lpSubKey);

  if (*(lpEnd - 1) != TEXT('\\')) {
    *lpEnd = TEXT('\\');
    lpEnd++;
    *lpEnd = TEXT('\0');
  }

  // Enumerate the keys

  dwSize = MAX_PATH;
  lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);

  if (lResult == ERROR_SUCCESS) {
    do {

      *lpEnd = TEXT('\0');
      StringCchCat(lpSubKey, MAX_PATH * 2, szName);

      if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
        break;
      }

      dwSize = MAX_PATH;

      lResult =
          RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);

    } while (lResult == ERROR_SUCCESS);
  }

  lpEnd--;
  *lpEnd = TEXT('\0');

  RegCloseKey(hKey);

  // Try again to delete the key.

  lResult = RegDeleteKey(hKeyRoot, lpSubKey);

  if (lResult == ERROR_SUCCESS)
    return TRUE;

  return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey) {
  TCHAR szDelKey[MAX_PATH * 2];

  StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
  return RegDelnodeRecurse(hKeyRoot, szDelKey);
}

std::string getexename() {
  char f[1024] = {};
  GetModuleFileNameA(nullptr, f, sizeof f);
  return f;
}

int execute(std::string cmd) {
  STARTUPINFOA info = {sizeof(info)};
  PROCESS_INFORMATION processInfo;
  int r = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW,
                         NULL, NULL, &info, &processInfo);
  if (!r) {
    throw std::runtime_error{"failed to execute - " + cmd};
  }
  return r;
}

std::string application_name() {
  std::string e = getexename();
  char buf[1024] = {0};
  LoadStringA(GetModuleHandleW(NULL), 11, buf, sizeof buf);
  return buf;
}

std::wstring toStdWString(const std::string &str) {
  return std::wstring(str.begin(), str.end());
}

std::string getappfolder() {
  auto e = getexename();
  e = e.substr(0, e.find_last_of('\\'));
  return e;
}

template <typename... T> void writelines(const char *srcfile, T... lines) {
  std::FILE *f = std::fopen(srcfile, "w");
  if (!f)
    return;
  std::string l;
  ((l += std::string(lines) + '\n'), ...);
  std::fputs(l.c_str(), f);
}

#ifdef NDEBUG
#define MAIN                                                                   \
  __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,              \
                    LPSTR lpCmdLine, int nShowCmd)
#else
#define MAIN main()
#endif

int MAIN try {
  auto exe = getexename();
  auto app = application_name();
  auto msgBoxRet = MessageBoxA(
      NULL, ("Do you really want to uninstall - " + app + "?").c_str(),
      "Uninstaller", MB_YESNO);
  if (msgBoxRet != IDYES)
    return 0;
  for (auto f : std::filesystem::recursive_directory_iterator{getappfolder()}) {
    if (f.path() != exe)
      std::filesystem::remove(f);
  }

  if (!RegDelnode(HKEY_LOCAL_MACHINE,
                  (LR"(Software\Microsoft\Windows\CurrentVersion\Uninstall\)" +
                   toStdWString(app))
                      .c_str())) {
    MessageBoxA(NULL, "Failed to Delete Registry Keys", "Uninstaller", MB_OK);
  }

  MessageBoxA(NULL, "Uninstallation finished", "Uninstaller", MB_OK);

  auto bat = std::filesystem::temp_directory_path() / "del.bat";

  execute(("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & rd /s /q \"" +
           getappfolder() + "\" & rmdir \"" + getappfolder() + "\""));

} catch (std::exception &e) {
  MessageBoxA(
      NULL,
      ("Uninstallation failed because - " + std::string(e.what())).c_str(),
      "Uninstaller", MB_OK);
}

// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces
//              to create and store a shortcut to the specified object.
//
// Returns the result of calling the member functions of the interfaces.
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the
//                Shell link, stored in the Comment field of the link
//                properties.

#define UNICODE 1
#define _UNICODE

#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "shobjidl.h"
#include "windows.h"
#include "winnls.h"

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc,
                   const wchar_t *workingdir) {
  HRESULT hres;
  IShellLink *psl;

  // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
  // has already been called.
  hres =
      CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
  if (SUCCEEDED(hres)) {
    IPersistFile *ppf;

    // Set the path to the shortcut target and add the description.
    psl->SetPath(lpszPathObj);
    psl->SetDescription(lpszDesc);
    psl->SetWorkingDirectory(workingdir);

    // Query IShellLink for the IPersistFile interface, used for saving the
    // shortcut in persistent storage.
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

    if (SUCCEEDED(hres)) {
      //      WCHAR wsz[MAX_PATH];

      //      // Ensure that the string is Unicode.
      //      MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

      // Add code here to check return value from MultiByteWideChar
      // for success.

      // Save the link by calling IPersistFile::Save.
      hres = ppf->Save(lpszPathLink, TRUE);
      ppf->Release();
    }
    psl->Release();
  }
  return hres;
}

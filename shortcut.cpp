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

#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "shobjidl.h"
#include "windows.h"
#include "winnls.h"

#include <gupta/cleanup.hpp>

int CreateLink(const char *srcFileName, const char *DestFileName,
               const char *Description) {
  HRESULT hres;
  IShellLinkA *psl;

  CoInitialize(NULL);
  SCOPE_EXIT { CoUninitialize(); };

  // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
  // has already been called.
  hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          IID_IShellLink, (LPVOID *)&psl);
  if (SUCCEEDED(hres)) {
    IPersistFile *ppf;

    // Set the path to the shortcut target and add the description.
    psl->SetPath(srcFileName);
    psl->SetDescription(Description);

    // Query IShellLink for the IPersistFile interface, used for saving the
    // shortcut in persistent storage.
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

    if (SUCCEEDED(hres)) {
      WCHAR wsz[MAX_PATH];

      // Ensure that the string is Unicode.
      MultiByteToWideChar(CP_ACP, 0, DestFileName, -1, wsz, MAX_PATH);

      // Add code here to check return value from MultiByteWideChar
      // for success.

      // Save the link by calling IPersistFile::Save.
      hres = ppf->Save(wsz, TRUE);
      ppf->Release();
    }
    psl->Release();
  }
  return hres;
}
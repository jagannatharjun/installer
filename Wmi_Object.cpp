#define _WIN32_DCOM
#include "wmi_object.h"

#include <Wbemidl.h>
#include <comdef.h>

#include <iostream>
#include <string>
#include <variant>
using namespace std;

void print_prop(WMI_Object::property_t v) {
  if (auto i = std::get_if<uint64_t>(&v))
    wcout << (*i) / (1024 * 1024 * 1024 * 1024ll * 1024 * 2) << std::endl;
  else if (auto j = std::get_if<std::wstring>(&v)) {
    wcout << *j << std::endl;
  }
}

// int main(int argc, char **argv) {
//  ComInitializer ComInitialize;
//  WMI_Services services;
//  // Win32_OS osinfo;
//  WMI_Object processinfo(L"Win32_Processor", services), osinfo(L"Win32_OperatingSystem",
//  services),
//      ginfo(L"Win32_VideoController", services);

//  print_prop(processinfo.get(L"Caption"));
//  print_prop(osinfo.get(L"Caption"));
//  print_prop(ginfo.get(L"Caption"));
//  return 0; // Program successfully completed.
//}

WMI_Object::WMI_Object(wstring ObjectClass, WMI_Services &Services) {

  //    // Step 5: --------------------------------------------------
  //    // Set security levels on the proxy -------------------------

  //    hres = CoSetProxyBlanket(pSvc,                        // Indicates the proxy to set
  //                             RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
  //                             RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
  //                             NULL,                        // Server principal name
  //                             RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
  //                             RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
  //                             NULL,                        // client identity
  //                             EOAC_NONE                    // proxy capabilities
  //    );

  //    if (FAILED(hres)) {
  //      cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
  //      pSvc->Release();
  //      pLoc->Release();
  //      CoUninitialize();
  //    }

  // Step 6: --------------------------------------------------
  // Use the IWbemServices pointer to make requests of WMI ----

  HRESULT hres;
  // For example, get the name of the operating system
  hres = Services.pSvc->ExecQuery(bstr_t(L"WQL"), bstr_t((L"SELECT * FROM " + ObjectClass).c_str()),
                                  WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,
                                  &pEnumerator);

  if (FAILED(hres)) {
    cout << "Query for operating system name failed."
         << " Error code = 0x" << hex << hres << endl;

    exit(1);
  }

  // Step 7: -------------------------------------------------
  // Get the data from the query in step 6 -------------------
}

WMI_Object::property_t WMI_Object::get(wstring property) {
  IWbemClassObject *pclsObj = NULL;
  ULONG uReturn = 0;

  property_t res;
  while (pEnumerator) {
    HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

    if (0 == uReturn) {
      break;
    }

    CIMTYPE type;
    VARIANT vtProp;
    // Get the value of the Name property
    hr = pclsObj->Get(property.c_str(), 0, &vtProp, &type, 0);

    wcerr << "Property - " << property << ": " << type << std::endl;

    if (type == CIM_SINT8 || type == CIM_UINT8)
      res = (uint64_t)(vtProp.cVal);
    else if (type == CIM_SINT16 || type == CIM_UINT16)
      res = (uint64_t)vtProp.iVal;
    else if (type == CIM_SINT32 || type == CIM_UINT32)
      res = (uint64_t)vtProp.lVal;
    else if (type == CIM_SINT64 || type == CIM_UINT64)
      res = (uint64_t)vtProp.llVal;
    else if (type == CIM_STRING)
      res = (std::wstring)vtProp.bstrVal;
    else {
      wcerr << "Unknown Variant Type = " << type << " for property " << property << std::endl;
    }
    VariantClear(&vtProp);

    pclsObj->Release();
    break;
  }
  return res;
}

WMI_Object::~WMI_Object() {
  // Cleanup
  // ========
  pEnumerator->Release();
}

WMI_Services::WMI_Services() {
  // Step 3: ---------------------------------------------------
  // Obtain the initial locator to WMI -------------------------

  HRESULT hres;

  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                          (LPVOID *)&pLoc);

  if (FAILED(hres)) {
    cout << "Failed to create IWbemLocator object."
         << " Err code = 0x" << hex << hres << endl;
  }

  // Step 4: -----------------------------------------------------
  // Connect to WMI through the IWbemLocator::ConnectServer method

  // Connect to the root\cimv2 namespace with
  // the current user and obtain pointer pSvc
  // to make IWbemServices calls.
  hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
                             NULL,                    // User name. NULL = current user
                             NULL,                    // User password. NULL = current
                             0,                       // Locale. NULL indicates current
                             NULL,                    // Security flags.
                             0,                       // Authority (for example, Kerberos)
                             0,                       // Context object
                             &pSvc                    // pointer to IWbemServices proxy

  );

  if (FAILED(hres)) {
    cout << "Could not connect. Error code = 0x" << hex << hres << endl;
  }

  cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;
}

WMI_Services::~WMI_Services() {
  pSvc->Release();
  pLoc->Release();
}

ComInitializer::ComInitializer() {
  HRESULT hres;
  hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres)) {
    cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
  }

  // Step 2: --------------------------------------------------
  // Set general COM security levels --------------------------

  hres = CoInitializeSecurity(NULL,
                              -1,                          // COM authentication
                              NULL,                        // Authentication services
                              NULL,                        // Reserved
                              RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
                              RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                              NULL,                        // Authentication info
                              EOAC_NONE,                   // Additional capabilities
                              NULL                         // Reserved
  );

  if (FAILED(hres)) {
    cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
  }
}

ComInitializer::~ComInitializer() { CoUninitialize(); }

WMI_Object::property_t getWmiProp(wstring ObjectClass, wstring ObjectProperty) {
  WMI_Object::property_t res;
  HRESULT hres;

  // Step 1: --------------------------------------------------
  // Initialize COM. ------------------------------------------

  //  hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  //  if (FAILED(hres)) {
  //    cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
  //    return res; // Program has failed.
  //  }

  // Step 2: --------------------------------------------------
  // Set general COM security levels --------------------------

  //  hres = CoInitializeSecurity(NULL,
  //                              -1,                          // COM authentication
  //                              NULL,                        // Authentication services
  //                              NULL,                        // Reserved
  //                              RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
  //                              RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
  //                              NULL,                        // Authentication info
  //                              EOAC_NONE,                   // Additional capabilities
  //                              NULL                         // Reserved
  //  );

  //  if (FAILED(hres)) {
  //    cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
  //    // CoUninitialize();
  //    return res; // Program has failed.
  //  }

  // Step 3: ---------------------------------------------------
  // Obtain the initial locator to WMI -------------------------

  IWbemLocator *pLoc = NULL;

  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                          (LPVOID *)&pLoc);

  if (FAILED(hres)) {
    cout << "Failed to create IWbemLocator object."
         << " Err code = 0x" << hex << hres << endl;
    //  CoUninitialize();
    return res; // Program has failed.
  }

  // Step 4: -----------------------------------------------------
  // Connect to WMI through the IWbemLocator::ConnectServer method

  IWbemServices *pSvc = NULL;

  // Connect to the root\cimv2 namespace with
  // the current user and obtain pointer pSvc
  // to make IWbemServices calls.
  hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
                             NULL,                    // User name. NULL = current user
                             NULL,                    // User password. NULL = current
                             0,                       // Locale. NULL indicates current
                             NULL,                    // Security flags.
                             0,                       // Authority (for example, Kerberos)
                             0,                       // Context object
                             &pSvc                    // pointer to IWbemServices proxy
  );

  if (FAILED(hres)) {
    cout << "Could not connect. Error code = 0x" << hex << hres << endl;
    pLoc->Release();
    // CoUninitialize();
    return res; // Program has failed.
  }

  // Step 5: --------------------------------------------------
  // Set security levels on the proxy -------------------------

  hres = CoSetProxyBlanket(pSvc,                        // Indicates the proxy to set
                           RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                           RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                           NULL,                        // Server principal name
                           RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
                           RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                           NULL,                        // client identity
                           EOAC_NONE                    // proxy capabilities
  );

  if (FAILED(hres)) {
    cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
    pSvc->Release();
    pLoc->Release();
    // CoUninitialize();
    return res; // Program has failed.
  }

  // Step 6: --------------------------------------------------
  // Use the IWbemServices pointer to make requests of WMI ----

  // For example, get the name of the operating system
  IEnumWbemClassObject *pEnumerator = NULL;
  hres = pSvc->ExecQuery(bstr_t(L"WQL"),
                         bstr_t((L"SELECT " + ObjectProperty + L"  FROM " + ObjectClass).c_str()),
                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

  if (FAILED(hres)) {
    cout << "Query for operating system name failed."
         << " Error code = 0x" << hex << hres << endl;
    pSvc->Release();
    pLoc->Release();
    //  CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 7: -------------------------------------------------
  // Get the data from the query in step 6 -------------------

  IWbemClassObject *pclsObj = NULL;
  ULONG uReturn = 0;

  while (pEnumerator) {
    HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

    if (0 == uReturn) {
      break;
    }

    VARIANT vtProp;
    CIMTYPE type;

    // Get the value of the Name property
    hr = pclsObj->Get(ObjectProperty.c_str(), 0, &vtProp, &type, 0);
    if (type == CIM_SINT8 || type == CIM_UINT8)
      res = (uint64_t)(vtProp.cVal);
    else if (type == CIM_SINT16 || type == CIM_UINT16)
      res = (uint64_t)vtProp.iVal;
    else if (type == CIM_SINT32 || type == CIM_UINT32)
      res = (uint64_t)vtProp.lVal;
    else if (type == CIM_SINT64 || type == CIM_UINT64)
      res = (uint64_t)vtProp.llVal;
    else if (type == CIM_STRING)
      res = (std::wstring)vtProp.bstrVal;
    else {
      wcerr << "Unknown Variant Type = " << type << " for property " << ObjectProperty << std::endl;
    }

    // wcerr << "ObjectProperty: " << vtProp.bstrVal;
    VariantClear(&vtProp);
    pclsObj->Release();

    break;
  }

  // Cleanup
  // ========

  pSvc->Release();
  pLoc->Release();
  pEnumerator->Release();
  //  CoUninitialize();

  return res; // Program successfully completed.
}

CpuUsage::CpuUsage() {
  // Get the local locator object
  CIMTYPE variant;
  VARIANT VT;

  CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                   (void **)&pWbemLocator);

  // Connect to the desired namespace

  HRESULT hr = WBEM_S_NO_ERROR;

  hr = pWbemLocator->ConnectServer(L"\\\\.\\root\\cimv2", // Namespace name
                                   NULL,                  // User name
                                   NULL,                  // Password
                                   NULL,                  // Locale
                                   0L,                    // Security flags
                                   NULL,                  // Authority
                                   NULL,                  // Wbem context
                                   &pNameSpace            // Namespace
  );

  if (SUCCEEDED(hr)) {
    // Set namespace security.
    IUnknown *pUnk = NULL;
    pNameSpace->QueryInterface(IID_IUnknown, (void **)&pUnk);

    hr = CoSetProxyBlanket(pNameSpace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hr)) {
      cout << "Cannot set proxy blanket. Error code: 0x" << hex << hr << endl;
      pNameSpace->Release();
    }

    hr = CoSetProxyBlanket(pUnk, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hr)) {
      cout << "Cannot set proxy blanket. Error code: 0x" << hex << hr << endl;
      pUnk->Release();
    }

    // Clean up the IUnknown.
    pUnk->Release();

    // Create a WMI Refresher and get a pointer to the
    // IWbemConfigureRefresher interface.
    CoCreateInstance(CLSID_WbemRefresher, NULL, CLSCTX_INPROC_SERVER, IID_IWbemRefresher,
                     (void **)&pRefresher);

    pRefresher->QueryInterface(IID_IWbemConfigureRefresher, (void **)&pConfig);

    IWbemClassObject *pObj = NULL;

    // Add the instance to be refreshed.
    pConfig->AddObjectByPath(pNameSpace, L"Win32_Processor.Name=\"WINWORD\"", 0L, NULL, &pObj,
                             NULL);
    if (FAILED(hr)) {
      cout << "Cannot add object. Error code: 0x" << hex << hr << endl;
      pNameSpace->Release();
    }

    // For quick property retrieval, use IWbemObjectAccess.
    pObj->QueryInterface(IID_IWbemObjectAccess, (void **)&pAcc);

    // This is not required.
    pObj->Release();

    // Get a property handle for the VirtualBytes property.
    pAcc->GetPropertyHandle(L"LoadPercentage", &variant, &loadPercentageHandle);

    // Clean up all the objects.
  }
}

CpuUsage::~CpuUsage() {
  pAcc->Release();
  pConfig->Release();
  pRefresher->Release();
  pNameSpace->Release();
  pWbemLocator->Release();
}

int CpuUsage::getCpuUsagePercent() {
  DWORD dwLoadPercentage;
  pRefresher->Refresh(0L);
  pAcc->ReadDWORD(loadPercentageHandle, &dwLoadPercentage);
  return dwLoadPercentage;
}

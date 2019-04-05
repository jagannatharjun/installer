#ifndef WMI_OBJECT_H
#define WMI_OBJECT_H

#include <string>
#include <variant>

struct IEnumWbemClassObject;
struct IWbemServices;
struct IWbemLocator;

struct ComInitializer {
  ComInitializer();
  ~ComInitializer();
};

struct WMI_Services {
  IWbemServices *pSvc = NULL;
  IWbemLocator *pLoc = NULL;

  WMI_Services();
  ~WMI_Services();
};

class WMI_Object {
public:
  using property_t = std::variant<std::wstring, uint64_t>;

  WMI_Object(std::wstring ObjectClass, WMI_Services &Services);
  property_t get(std::wstring property);
  ~WMI_Object();

private:
  IEnumWbemClassObject *pEnumerator = NULL;
};

WMI_Object::property_t getWmiProp(std::wstring ObjectClass, std::wstring ObjectProperty);

class IWbemRefresher;
class IWbemConfigureRefresher;
class IWbemObjectAccess;

class CpuUsage {
  long loadPercentageHandle = 0;

  IWbemServices *pNameSpace = NULL;
  IWbemLocator *pWbemLocator = NULL;

  IWbemRefresher *pRefresher = NULL;
  IWbemConfigureRefresher *pConfig = NULL;
  IWbemObjectAccess *pAcc = NULL;

public:
  CpuUsage();
  ~CpuUsage();
  int getCpuUsagePercent();
};

#endif // WMI_OBJECT_H

#ifndef APPUTILS_HPP
#define APPUTILS_HPP

#include <string>
#include <ctime>
#include <map>

class AppUtils
{
  public:
    enum OSType
    {
      NT=0,
      UNIX=1,
      UNKNOWN=2
    };
    static std::string OSNAME()
    {
      #ifdef _WIN32
      return "Windows";
      #elif _WIN64
      return "Windows";
      #elif __APPLE__ || __MACH__
      return "Mac OSX";
      #elif __linux__
      return "Linux";
      #elif __FreeBSD__
      return "FreeBSD";
      #elif __unix || __unix__
      return "Unix";
      #else
      return "Other";
      #endif
    };

    static int OSTYPE()
    {
      std::map<std::string,int> OSMap {{"FreeBSD",AppUtils::OSType::UNIX},{"Linux",AppUtils::OSType::UNIX},{"Mac OSX", AppUtils::OSType::UNIX},{"Unix",AppUtils::OSType::UNIX},{"Windows",AppUtils::OSType::NT},{"Other",AppUtils::OSType::UNKNOWN}};
      std::string os_name = AppUtils::OSNAME();
      return (OSMap.count(os_name)>0) ? OSMap[os_name] : AppUtils::OSType::UNKNOWN; 
    };


    static std::string timestamp(const char* format)
    {
      char time_formatted[100];
      const std::time_t ctime_raw = std::time(nullptr);
      std::strftime(time_formatted,sizeof(time_formatted),format,std::localtime(&ctime_raw));
      return std::string(time_formatted);
    };

    static std::string HOMEDIR()
    {
      if(AppUtils::OSTYPE()==AppUtils::OSType::NT)
      {
        const char* homedrive = getenv("HOMEDRIVE");
        const char* homepath = getenv("HOMEPATH");
        return (homedrive == NULL || homepath == NULL) ? "" : std::string(homedrive)+std::string(homepath);
      }

      if(AppUtils::OSTYPE()==AppUtils::OSType::UNIX)
      {
        const char* homedir = getenv("HOME");
        return (homedir == NULL) ? "" : std::string(homedir);
      }

      return "";
    };

};

#endif

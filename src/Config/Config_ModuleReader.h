// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_XMLModuleReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_MODULEREADER_H_
#define CONFIG_MODULEREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <map>
#include <list>
#include <string>

class Config_ModuleReader : public Config_XMLReader
{
  enum PluginType {
    Binary = 0,
    Intrenal = 1,
    Python = 2
  };
  enum PluginPlatform {
    All = 0,
    OpenParts = 1,
    Salome = 2
  };

 public:
  CONFIG_EXPORT Config_ModuleReader(const char* theEventGenerated = 0);
  CONFIG_EXPORT virtual ~Config_ModuleReader();

  CONFIG_EXPORT const std::map<std::string, std::string>& featuresInFiles() const;

  CONFIG_EXPORT std::string getModuleName();

  CONFIG_EXPORT static void loadPlugin(const std::string thePluginName);
  /// loads the library with specific name, appends "lib*.dll" or "*.so" depending on the platform
  CONFIG_EXPORT static void loadLibrary(const std::string theLibName);
  /// loads the python module with specified name
  CONFIG_EXPORT static void loadScript(const std::string theFileName);

 protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  bool isAvaliableOnThisPlatform(const std::string& thePluginPlatform);
  std::list<std::string> importPlugin(const std::string& thePluginLibrary,
                                      const std::string& thePluginFile);
  std::string addPlugin(const std::string& aPluginLibrary,
                        const std::string& aPluginScript,
                        const std::string& aPluginConf);

 private:
  std::map<std::string, std::string> myFeaturesInFiles;
  static std::map<std::string, PluginType> myPluginTypes;
  const char* myEventGenerated;
  bool myHaveSalome;

};

#endif /* CONFIG_XMLMODULEREADER_H_ */

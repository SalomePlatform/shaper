/*
 * Config_XMLModuleReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_MODULEREADER_H_
#define CONFIG_MODULEREADER_H_

#include <Config.h>
#include <Config_XMLReader.h>

#include <map>
#include <list>
#include <string>


class Config_ModuleReader: public Config_XMLReader
{

public:
  CONFIG_EXPORT Config_ModuleReader(const char* theEventGenerated = 0);
  CONFIG_EXPORT virtual ~Config_ModuleReader();

  CONFIG_EXPORT const std::map<std::string, std::string>& featuresInFiles() const;

  CONFIG_EXPORT std::string getModuleName();

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  std::list<std::string> importPlugin(const std::string& thePluginFile,
                                      const std::string& thePluginLibrary);

private:
  std::map<std::string, std::string> myFeaturesInFiles;
  const char* myEventGenerated;

};

#endif /* CONFIG_XMLMODULEREADER_H_ */

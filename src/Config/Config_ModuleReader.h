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
#include <string>


class CONFIG_EXPORT Config_ModuleReader: public Config_XMLReader
{

public:
  Config_ModuleReader();
  virtual ~Config_ModuleReader();

  void setAutoImport(bool enabled);
  const std::map<std::string, std::string>& plugins() const;

  std::string getModuleName();

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void importPlugin(const std::string& thePluginName,
                    const std::string& thePluginLibrary = "");

private:
  bool myIsAutoImport;
  std::map<std::string, std::string> myPluginsMap;


};

#endif /* CONFIG_XMLMODULEREADER_H_ */

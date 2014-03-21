/*
 * Config_XMLModuleReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_MODULEREADER_H_
#define CONFIG_MODULEREADER_H_

#include <Config_XMLReader.h>

#include <list>

class CONFIG_EXPORT Config_ModuleReader: public Config_XMLReader
{

public:
  Config_ModuleReader();
  virtual ~Config_ModuleReader();

  void setAutoImport(bool enabled);
  const std::list<std::string>& pluginsList() const;

  std::string getModuleName();

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void importPlugin(const std::string& thePluginName);

private:
  bool m_isAutoImport;
  std::list<std::string> m_pluginsList;

};

#endif /* CONFIG_XMLMODULEREADER_H_ */

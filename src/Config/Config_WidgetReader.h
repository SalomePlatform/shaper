/*
 * Config_WidgetReader.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef CONFIG_WIDGETREADER_H_
#define CONFIG_WIDGETREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <map>
#include <string>


class Config_WidgetReader: public Config_XMLReader
{
public:
  CONFIG_EXPORT Config_WidgetReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_WidgetReader();

  CONFIG_EXPORT std::string featureWidgetCfg(const std::string& theFeatureName);
  CONFIG_EXPORT std::string featureDescription(const std::string& theFeatureName);

protected:
  void processNode(xmlNodePtr theNode);
  bool processChildren(xmlNodePtr theNode);
  std::string dumpNode(xmlNodePtr theNode);

private:
  std::map<std::string, std::string> myWidgetCache;
  std::map<std::string, std::string> myDescriptionCache;

};

#endif /* CONFIG_WIDGETREADER_H_ */

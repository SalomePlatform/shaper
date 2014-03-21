/*
 * Config_FeatureReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_FEATUREREADER_H_
#define CONFIG_FEATUREREADER_H_

#include <Config_XMLReader.h>

class CONFIG_EXPORT Config_FeatureReader: public Config_XMLReader
{
public:
  Config_FeatureReader(const std::string& theXmlFile);
  virtual ~Config_FeatureReader();

  std::string featureWidgetCfg(std::string theFeatureName);

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void fillFeature(xmlNodePtr theRoot, Config_FeatureMessage& outFeatureMessage);

  std::string m_lastWorkbench;
  std::string m_lastGroup;

  bool m_fetchWidgetCfg;
  std::string m_widgetCfg;
};

#endif /* CONFIG_FEATUREREADER_H_ */

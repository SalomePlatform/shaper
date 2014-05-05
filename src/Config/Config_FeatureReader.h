/*
 * Config_FeatureReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_FEATUREREADER_H_
#define CONFIG_FEATUREREADER_H_

#include <Config.h>
#include <Config_XMLReader.h>

#include <string>
#include <list>

class Config_FeatureMessage;

class Config_FeatureReader: public Config_XMLReader
{
public:
  Config_FeatureReader(const std::string& theXmlFile,
                       const std::string& theLibraryName,
                       const char* theEventGenerated = 0);
  virtual ~Config_FeatureReader();

  std::list<std::string> features() const;

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void fillFeature(xmlNodePtr theRoot, Config_FeatureMessage& outFeatureMessage);

private:
  std::string myLastWorkbench;
  std::string myLastGroup;
  std::string myLibraryName;

  std::list<std::string> myFeatures;
  /// event generated on feature data sending, by default it is EVENT_FEATURE_LOADED
  const char* myEventGenerated;
};

#endif /* CONFIG_FEATUREREADER_H_ */

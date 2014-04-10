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


class Config_FeatureMessage;

class Config_FeatureReader: public Config_XMLReader
{
public:
  Config_FeatureReader(const std::string& theXmlFile,
                       const std::string& theLibraryName = "");
  virtual ~Config_FeatureReader();

protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void fillFeature(xmlNodePtr theRoot, Config_FeatureMessage& outFeatureMessage);

private:
  std::string myLastWorkbench;
  std::string myLastGroup;
  std::string myLibraryName;
};

#endif /* CONFIG_FEATUREREADER_H_ */

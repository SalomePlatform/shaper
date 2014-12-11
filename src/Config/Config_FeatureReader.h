// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_FeatureReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_FEATUREREADER_H_
#define CONFIG_FEATUREREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>
#include <memory>

#include <string>
#include <list>
#include <map>

class Config_FeatureMessage;

class Config_FeatureReader : public Config_XMLReader
{
 public:
  Config_FeatureReader(const std::string& theXmlFile, const std::string& theLibraryName,
                       const char* theEventGenerated = 0);
  virtual ~Config_FeatureReader();

  std::list<std::string> features() const;

 protected:
  void processNode(xmlNodePtr aNode);
  bool processChildren(xmlNodePtr aNode);

  void fillFeature(xmlNodePtr theRoot, 
                   const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage);

  void storeAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  std::string restoreAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  std::string restoreAttribute(const char* theNodeName, const char* theNodeAttribute);

 private:
  /// A map to store all parent's attributes.
  /// The key has from "Node_Name:Node_Attribute"
  std::map<std::string, std::string> myParentAttributes;
  std::string myLibraryName;

  std::list<std::string> myFeatures;
  /// event generated on feature data sending, by default it is Config_FeatureMessage::GUI_EVENT()
  const char* myEventGenerated;
  bool myIsProcessWidgets;
};

#endif /* CONFIG_FEATUREREADER_H_ */

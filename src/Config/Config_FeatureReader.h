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

/*!
 * \class Config_FeatureReader
 * \ingroup Config
 * \brief Class to process feature's xml definition.
 */
class Config_FeatureReader : public Config_XMLReader
{
 public:
  /// Constructor
  Config_FeatureReader(const std::string& theXmlFile,
                       const std::string& theLibraryName,
                       const char* theEventGenerated = 0);
  virtual ~Config_FeatureReader();
  /// Returns list of all features defined in reader's file
  std::list<std::string> features() const;

 protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr aNode);

  /// Overloaded method. Clears attribute cache on extit from attribute's node
  virtual void cleanup(xmlNodePtr aNode);

  /// Overloaded method. Defines if the given node should be parsed recursively
  virtual bool processChildren(xmlNodePtr aNode);

  /// Fills feature message
  void fillFeature(xmlNodePtr theRoot, 
                   const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage);

  /// Stores an attribute in internal map for later use.
  /// Key is "Node_Name:Node_Attribute" and value is getProperty(theNodeAttribute)
  void storeAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  /// Restores an attribute from internal map.
  std::string restoreAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  /// Restores an attribute from internal map.
  std::string restoreAttribute(const char* theNodeName, const char* theNodeAttribute);

  bool cleanupAttribute(xmlNodePtr theNode, const char* theNodeAttribute);
  bool cleanupAttribute(const char* theNodeName, const char* theNodeAttribute);

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

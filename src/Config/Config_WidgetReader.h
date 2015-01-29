// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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

/*!
 * \class Config_WidgetReader
 * \ingroup Config
 * \brief Class that dumps xml definitions of widgets for
 * further processing in the WidgetFactory
 */
class Config_WidgetReader : public Config_XMLReader
{
 public:
  /*!
   * Constructor
   * \param theXmlFile - full path to the xml file which will be processed by the reader
   */
  CONFIG_EXPORT Config_WidgetReader(const std::string& theXmlFile);
  CONFIG_EXPORT virtual ~Config_WidgetReader();

  /// Extract feature's widget configuration from local cache, stored on node processing
  CONFIG_EXPORT std::string featureWidgetCfg(const std::string& theFeatureName);
  /// Extract feature's description from local cache, stored on node processing
  CONFIG_EXPORT std::string featureDescription(const std::string& theFeatureName);

 protected:
  /// Overloaded method. Defines how to process each node
  void processNode(xmlNodePtr theNode);
  /// Overloaded method. Defines if the given node should be parsed recursively
  bool processChildren(xmlNodePtr theNode);
  /// Extracts xml definition of the given node and it's children
  std::string dumpNode(xmlNodePtr theNode);
  /// Replace all "source" nodes with their content (used before dumping nodes)
  void resolveSourceNodes(xmlNodePtr theNode);

 private:
  std::map<std::string, std::string> myWidgetCache;
  std::map<std::string, std::string> myDescriptionCache;

};

#endif /* CONFIG_WIDGETREADER_H_ */

/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_FeatureReader.h>

#include <Event_Loop.h>

#include <libxml\parser.h>
#include <libxml\tree.h>

#ifdef _DEBUG
#include <iostream>
#endif

//Hardcoded xml entities
// * Nodes
const static char* NODE_WORKBENCH = "workbench";
const static char* NODE_GROUP = "group";
const static char* NODE_FEATURE = "feature";

// * Properties
const static char* _ID = "id";
//const static char* WORKBENCH_ID = "id";
//const static char* GROUP_ID = "id";
//const static char* FEATURE_ID = "id";
const static char* FEATURE_TEXT = "text";
const static char* FEATURE_TOOLTIP = "tooltip";
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_KEYSEQUENCE = "keysequence";

Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile)
    : Config_XMLReader(theXmlFile),
      myFetchWidgetCfg(false)
{
  myLibraryName = "";

#ifdef _DEBUG
  if (!Event_Loop::loop()) {
    std::cout << "Config_FeatureReader::importWorkbench: "
        << "No event loop registered" << std::endl;
  }
#endif
}

Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName),
      myFetchWidgetCfg(false)
{
#ifdef _DEBUG
  if (!Event_Loop::loop()) {
    std::cout << "Config_FeatureReader::importWorkbench: "
        << "No event loop registered" << std::endl;
  }
#endif
}

Config_FeatureReader::~Config_FeatureReader()
{
}

std::string Config_FeatureReader::featureWidgetCfg(std::string theFeatureName)
{
  myFetchWidgetCfg = true;
  readAll();
  myFetchWidgetCfg = false;
  return myWidgetCfg;
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  static Event_ID aMenuItemEvent = Event_Loop::eventByName("RegisterFeature");
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    if (myFetchWidgetCfg) {
      xmlBufferPtr buffer = xmlBufferCreate();
      int size = xmlNodeDump(buffer, theNode->doc, theNode, 0, 1);
      myWidgetCfg = std::string((char*) buffer->content);
    } else {
      Event_Loop* aEvLoop = Event_Loop::loop();
      Config_FeatureMessage aMessage(aMenuItemEvent, this);
      fillFeature(theNode, aMessage);
      aEvLoop->send(aMessage);
    }
  }
  //The m_last* variables always defined before fillFeature() call. XML is a tree.
  if (isNode(theNode, NODE_GROUP, NULL)) {
    myLastGroup = getProperty(theNode, _ID);
  }
  if (isNode(theNode, NODE_WORKBENCH, NULL)) {
    myLastWorkbench = getProperty(theNode, _ID);
  }
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
}

void Config_FeatureReader::fillFeature(xmlNodePtr theRoot, Config_FeatureMessage& outFtMessage)
{
  outFtMessage.setId(getProperty(theRoot, _ID));
  outFtMessage.setText(getProperty(theRoot, FEATURE_TEXT));
  outFtMessage.setTooltip(getProperty(theRoot, FEATURE_TOOLTIP));
  outFtMessage.setIcon(getProperty(theRoot, FEATURE_ICON));
  outFtMessage.setKeysequence(getProperty(theRoot, FEATURE_KEYSEQUENCE));
  outFtMessage.setGroupId(myLastGroup);
  outFtMessage.setWorkbenchId(myLastWorkbench);
  outFtMessage.setPluginLibrary(myLibraryName);
}

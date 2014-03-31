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
    : Config_XMLReader(theXmlFile), m_fetchWidgetCfg(false)
{
#ifdef _DEBUG
  if(!Event_Loop::Loop()) {
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
  m_fetchWidgetCfg = true;
  readAll();
  m_fetchWidgetCfg = false;
  return m_widgetCfg;
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    if (m_fetchWidgetCfg) {
      xmlBufferPtr buffer = xmlBufferCreate();
      int size = xmlNodeDump(buffer, theNode->doc, theNode, 0, 1);
      m_widgetCfg = std::string((char*) buffer->content);
    } else {
      Event_Loop* aEvLoop = Event_Loop::Loop();
      Config_FeatureMessage aMessage(aEvLoop->EventByName("menu_item"), this);
      fillFeature(theNode, aMessage);
      aEvLoop->Send(aMessage);
    }
  }
  //The m_last* variables always defined before fillFeature() call. XML is a tree.
  if (isNode(theNode, NODE_GROUP, NULL)) {
    m_lastGroup = getProperty(theNode, _ID);
  }
  if (isNode(theNode, NODE_WORKBENCH, NULL)) {
    m_lastWorkbench = getProperty(theNode, _ID);
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
  outFtMessage.setGroupId(m_lastGroup);
  outFtMessage.setWorkbenchId(m_lastWorkbench);

}

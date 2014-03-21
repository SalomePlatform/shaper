/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_FeatureReader.h>

#include <Event_Loop.hxx>

#include <libxml\parser.h>
#include <libxml\tree.h>

#ifdef _DEBUG
#include <iostream>
#endif

//Hardcoded xml entities
// * Nodes

// * Properties
const static char* FEATURE_ID = "id";
const static char* FEATURE_TEXT = "text";
const static char* FEATURE_TOOLTIP = "tooltip";
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_KEYSEQUENCE = "keysequence";
const static char* FEATURE_GROUP_NAME = "name";




Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile)
    : Config_XMLReader(theXmlFile)
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

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  if(isNode(theNode,"feature")) {
    Event_Loop* aEvLoop = Event_Loop::Loop();
    Config_FeatureMessage aMessage(aEvLoop->EventByName("Feature"), this);
    fillFeature(theNode, aMessage);
    xmlNodePtr aGroupNode = theNode->parent;
    if(aGroupNode) {
      std::string aGroupName = getProperty(aGroupNode, FEATURE_GROUP_NAME);
      aMessage.m_group = aGroupName;
    }
    aEvLoop->Send(aMessage);
  }
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, "workbench")
      || isNode(theNode, "group");
//      || isNode(theNode, "feature");
}

void Config_FeatureReader::fillFeature(xmlNodePtr theRoot,
                                       Config_FeatureMessage& outFtMessage)
{
  outFtMessage.m_id = getProperty(theRoot, FEATURE_ID);
  outFtMessage.m_text = getProperty(theRoot, FEATURE_TEXT);
  outFtMessage.m_tooltip = getProperty(theRoot, FEATURE_TOOLTIP);
  outFtMessage.m_icon = getProperty(theRoot, FEATURE_ICON);
  outFtMessage.m_keysequence = getProperty(theRoot, FEATURE_KEYSEQUENCE);
}

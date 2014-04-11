/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_FeatureMessage.h>
#include <Config_FeatureReader.h>
#include <Event_Message.h>
#include <Event_Loop.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <string>

#ifdef _DEBUG
#include <iostream>
#endif


Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName)
{
}

Config_FeatureReader::~Config_FeatureReader()
{
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  static Event_ID aMenuItemEvent = Event_Loop::eventByName("FeatureEvent");
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    Event_Loop* aEvLoop = Event_Loop::loop();
    Config_FeatureMessage aMessage(aMenuItemEvent, this);
    fillFeature(theNode, aMessage);
    aEvLoop->send(aMessage);
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

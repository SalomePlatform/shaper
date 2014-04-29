/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_FeatureReader.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <string>

#ifdef _DEBUG
#include <iostream>
#endif


Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName,
                                           const char* theEventGenerated)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName),
      myEventGenerated(theEventGenerated ? theEventGenerated : "FeatureEvent")
{
}

Config_FeatureReader::~Config_FeatureReader()
{
}

std::list<std::string> Config_FeatureReader::features() const
{
  return myFeatures;
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  Events_ID aMenuItemEvent = Events_Loop::eventByName(myEventGenerated);
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    Events_Loop* aEvLoop = Events_Loop::loop();
    Config_FeatureMessage aMessage(aMenuItemEvent, this);
    fillFeature(theNode, aMessage);
    myFeatures.push_back(getProperty(theNode, _ID));
    //If a feature has xml definition for it's widget:
    aMessage.setUseInput(hasChild(theNode));
    aEvLoop->send(aMessage);
  }
  //The m_last* variables always defined before fillFeature() call. XML is a tree.
  if (isNode(theNode, NODE_GROUP, NULL)) {
    myLastGroup = getProperty(theNode, _ID);
  }
  if (isNode(theNode, NODE_WORKBENCH, NULL)) {
    myLastWorkbench = getProperty(theNode, _ID);
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
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
  outFtMessage.setNestedFeatures(getProperty(theRoot, FEATURE_NESTED));
}

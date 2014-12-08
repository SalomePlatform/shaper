/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_AttributeMessage.h>
#include <Config_FeatureReader.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <string>
#include <algorithm>
#include <list>

#ifdef _DEBUG
#include <iostream>
#endif

Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName,
                                           const char* theEventGenerated)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName),
      myEventGenerated(theEventGenerated ? theEventGenerated : Config_FeatureMessage::GUI_EVENT()),
      myIsProcessWidgets(theEventGenerated != NULL)
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
    storeAttribute(theNode, _ID);
    std::shared_ptr<Config_FeatureMessage> aMessage(new Config_FeatureMessage(aMenuItemEvent, this));
    fillFeature(theNode, aMessage);
    myFeatures.push_back(getProperty(theNode, _ID));
    //If a feature has xml definition for it's widget:
    aMessage->setUseInput(hasChild(theNode));
    Events_Loop::loop()->send(aMessage);
    //The m_last* variables always defined before fillFeature() call. XML is a tree.
  } else if (isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL)) {
    storeAttribute(theNode, _ID);
    storeAttribute(theNode, WORKBENCH_DOC);
  } else if (myIsProcessWidgets && isWidgetNode(theNode)) {
    std::shared_ptr<Config_AttributeMessage> aMessage(new Config_AttributeMessage(aMenuItemEvent, this));
    aMessage->setFeatureId(restoreAttribute(NODE_FEATURE, _ID));
    std::string anAttributeID = getProperty(theNode, _ID);
    if (!anAttributeID.empty()) {
      aMessage->setAttributeId(anAttributeID);
      aMessage->setObligatory(getBooleanAttribute(theNode, ATTRIBUTE_OBLIGATORY, true));
      aMessage->setConcealment(getBooleanAttribute(theNode, ATTRIBUTE_CONCEALMENT, false));
      Events_Loop::loop()->send(aMessage);
    }
  }
  //Process SOURCE, VALIDATOR nodes.
  Config_XMLReader::processNode(theNode);
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  bool result = isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
  if(!result && myIsProcessWidgets) {
    result = isNode(theNode, NODE_FEATURE, NULL);
  }
  return result;
}

void Config_FeatureReader::fillFeature(xmlNodePtr theNode, 
  const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage)
{
  outFeatureMessage->setId(getProperty(theNode, _ID));
  outFeatureMessage->setPluginLibrary(myLibraryName);
  outFeatureMessage->setNestedFeatures(getProperty(theNode, FEATURE_NESTED));

  bool isInternal = getBooleanAttribute(theNode, ATTRIBUTE_INTERNAL, false);
  outFeatureMessage->setInternal(isInternal);
  if (isInternal) {
    //Internal feature has no visual representation.
    return;
  }
  outFeatureMessage->setText(getProperty(theNode, FEATURE_TEXT));
  outFeatureMessage->setTooltip(getProperty(theNode, FEATURE_TOOLTIP));
  outFeatureMessage->setIcon(getProperty(theNode, FEATURE_ICON));
  outFeatureMessage->setKeysequence(getProperty(theNode, FEATURE_KEYSEQUENCE));
  outFeatureMessage->setGroupId(restoreAttribute(NODE_GROUP, _ID));
  outFeatureMessage->setWorkbenchId(restoreAttribute(NODE_WORKBENCH, _ID));
  // Get document kind of a feature, if empty set workbench's kind (might be empty too)
  std::string aDocKind = getProperty(theNode, WORKBENCH_DOC);
  if(aDocKind.empty()) {
    aDocKind = restoreAttribute(NODE_WORKBENCH, WORKBENCH_DOC);
  }
  outFeatureMessage->setDocumentKind(aDocKind);
}

void Config_FeatureReader::storeAttribute(xmlNodePtr theNode,
                                          const char* theNodeAttribute)
{
  std::string aKey = getNodeName(theNode) + ":" + std::string(theNodeAttribute);
  std::string aValue = getProperty(theNode, theNodeAttribute);
  if(!aValue.empty()) {
    myParentAttributes[aKey] = aValue;
  }
}

std::string Config_FeatureReader::restoreAttribute(xmlNodePtr theNode,
                                                   const char* theNodeAttribute)
{
  return restoreAttribute(getNodeName(theNode).c_str(), theNodeAttribute);
}
std::string Config_FeatureReader::restoreAttribute(const char* theNodeName,
                                                   const char* theNodeAttribute)
{
  std::string aKey = std::string(theNodeName) + ":" + std::string(theNodeAttribute);
  std::string result = "";
  if(myParentAttributes.find(aKey) != myParentAttributes.end()) {
    result = myParentAttributes[aKey];
  }
  return result;
}

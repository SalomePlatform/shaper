// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
  } else if (myIsProcessWidgets) {
    // widgets, like shape_selector or containers, like toolbox
    if (isAttributeNode(theNode)) {
      std::shared_ptr<Config_AttributeMessage> aMessage(new Config_AttributeMessage(aMenuItemEvent, this));
      aMessage->setFeatureId(restoreAttribute(NODE_FEATURE, _ID));
      std::string anAttributeID = getProperty(theNode, _ID);
      if (!anAttributeID.empty()) {
        aMessage->setAttributeId(anAttributeID);
        aMessage->setObligatory(getBooleanAttribute(theNode, ATTR_OBLIGATORY, true));
        aMessage->setConcealment(getBooleanAttribute(theNode, ATTR_CONCEALMENT, false));
        // nested "paged" widgets are not allowed, this issue may be resolved here:
        if (hasParent(theNode, WDG_SWITCH_CASE, WDG_TOOLBOX_BOX, NULL)) {
          const char* kWdgCase = hasParent(theNode, WDG_SWITCH_CASE, NULL)
                                 ? WDG_SWITCH_CASE
                                 : WDG_TOOLBOX_BOX;
          const char* kWdgSwitch = hasParent(theNode, WDG_SWITCH_CASE, NULL)
                                   ? WDG_SWITCH
                                   : WDG_TOOLBOX;
          aMessage->setCaseId(restoreAttribute(kWdgCase, _ID));
          aMessage->setSwitchId(restoreAttribute(kWdgSwitch, _ID));
        }
        Events_Loop::loop()->send(aMessage);
      }
    // container pages, like "case" or "box"
    } else if (isNode(theNode, WDG_SWITCH, WDG_SWITCH_CASE, WDG_SWITCH, WDG_TOOLBOX, NULL)) {
      storeAttribute(theNode, _ID); // save case:caseId (or box:boxId)
    }
  }
  //Process SOURCE, VALIDATOR nodes.
  Config_XMLReader::processNode(theNode);
}

void Config_FeatureReader::cleanup(xmlNodePtr theNode)
{
  if (isNode(theNode, WDG_SWITCH, WDG_SWITCH_CASE, WDG_SWITCH, WDG_TOOLBOX, NULL)) {
    // cleanup id of cases when leave case node
    cleanupAttribute(theNode, _ID);
  }
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  bool result = isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
  if(!result && myIsProcessWidgets) {
    result = isNode(theNode, NODE_FEATURE, 
                             WDG_TOOLBOX, WDG_TOOLBOX_BOX,
                             WDG_SWITCH, WDG_SWITCH_CASE, NULL);
  }
  return result;
}

void Config_FeatureReader::fillFeature(xmlNodePtr theFeatureNode,
  const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage)
{
  outFeatureMessage->setId(getProperty(theFeatureNode, _ID));
  outFeatureMessage->setPluginLibrary(myLibraryName);
  outFeatureMessage->setNestedFeatures(getProperty(theFeatureNode, FEATURE_NESTED));
  outFeatureMessage->setActionsWhenNested(getNormalizedProperty(theFeatureNode, FEATURE_WHEN_NESTED));

  bool isInternal = getBooleanAttribute(theFeatureNode, ATTR_INTERNAL, false);
  outFeatureMessage->setInternal(isInternal);
  if (isInternal) {
    //Internal feature has no visual representation.
    return;
  }
  outFeatureMessage->setText(getProperty(theFeatureNode, FEATURE_TEXT));
  outFeatureMessage->setTooltip(getProperty(theFeatureNode, FEATURE_TOOLTIP));
  outFeatureMessage->setIcon(getProperty(theFeatureNode, FEATURE_ICON));
  outFeatureMessage->setKeysequence(getProperty(theFeatureNode, FEATURE_KEYSEQUENCE));
  outFeatureMessage->setGroupId(restoreAttribute(NODE_GROUP, _ID));
  outFeatureMessage->setWorkbenchId(restoreAttribute(NODE_WORKBENCH, _ID));
  // Get document kind of a feature, if empty set workbench's kind (might be empty too)
  std::string aDocKind = getProperty(theFeatureNode, FEATURE_DOC);
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

bool Config_FeatureReader::cleanupAttribute(xmlNodePtr theNode,
                                            const char* theNodeAttribute)
{
  return cleanupAttribute(getNodeName(theNode).c_str(), theNodeAttribute);
}

bool Config_FeatureReader::cleanupAttribute(const char* theNodeName,
                                            const char* theNodeAttribute)
{
  std::string aKey = std::string(theNodeName) + ":" + std::string(theNodeAttribute);
  bool result = false;
  std::map<std::string, std::string>::iterator anEntry = myParentAttributes.find(aKey);
  if( anEntry != myParentAttributes.end()) {
    myParentAttributes.erase(anEntry);
    result = true;
  }
  return result;
}


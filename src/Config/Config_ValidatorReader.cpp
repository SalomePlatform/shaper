// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_ValidatorReader.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: sbh
 */

#include <Config_ValidatorReader.h>
#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_ValidatorMessage.h>
#include <Config_SelectionFilterMessage.h>
#include <Config_PropManager.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <fstream>

#ifdef _DEBUG
#include <iostream>
#endif

Config_ValidatorReader::Config_ValidatorReader(const std::string& theXmlFileName)
: Config_XMLReader(theXmlFileName)
{
}

Config_ValidatorReader::~Config_ValidatorReader()
{
}

void Config_ValidatorReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_VALIDATOR, NULL)) {
    processValidator(theNode);
  } else if (isNode(theNode, NODE_SELFILTER, NULL)) {
    processSelectionFilter(theNode);
  } else if (isNode(theNode, NODE_FEATURE, NULL)) {
    storeAttribute(theNode, _ID);
  } else if (isWidgetNode(theNode)) {
    storeAttribute(theNode, _ID);
    // Store widget name to restore it's id on validator/selector processing
    myCurrentWidget = getNodeName(theNode);
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

void Config_ValidatorReader::cleanup(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    cleanupAttribute(theNode, _ID);
  } else if (isWidgetNode(theNode)) {
    cleanupAttribute(NODE_XMLPARENT, _ID);
    // Cleanup widget name when leave the widget node.
    myCurrentWidget = "";
  }
}

bool Config_ValidatorReader::processChildren(xmlNodePtr aNode)
{
  return true;
}

void Config_ValidatorReader::processValidator(xmlNodePtr theNode)
{
  Events_ID aValidatoEvent = Events_Loop::eventByName(EVENT_VALIDATOR_LOADED);
  Events_Loop* aEvLoop = Events_Loop::loop();
  std::shared_ptr<Config_ValidatorMessage> 
    aMessage(new Config_ValidatorMessage(aValidatoEvent, this));
  std::string aValidatorId;
  std::list<std::string> aParameters;
  getParametersInfo(theNode, aValidatorId, aParameters);
  aMessage->setValidatorId(aValidatorId);
  aMessage->setValidatorParameters(aParameters);
  std::string aFeatureId = restoreAttribute(NODE_FEATURE, _ID);
  aMessage->setFeatureId(aFeatureId);
  // parent is attribute (widget)
  if (!myCurrentWidget.empty()) {
    std::string aParentId = restoreAttribute(myCurrentWidget.c_str(), _ID);
    aMessage->setAttributeId(aParentId);
  }
  aEvLoop->send(aMessage);
}

void Config_ValidatorReader::processSelectionFilter(xmlNodePtr theNode)
{
  Events_ID aFilterEvent = Events_Loop::eventByName(EVENT_SELFILTER_LOADED);
  Events_Loop* aEvLoop = Events_Loop::loop();
  std::shared_ptr<Config_SelectionFilterMessage> aMessage(
      new Config_SelectionFilterMessage(aFilterEvent, this));
  std::string aSelectionFilterId;
  std::list<std::string> aParameters;
  getParametersInfo(theNode, aSelectionFilterId, aParameters);
  aMessage->setSelectionFilterId(aSelectionFilterId);
  aMessage->setFilterParameters(aParameters);
  std::string aFeatureId = restoreAttribute(NODE_FEATURE, _ID);
  aMessage->setFeatureId(aFeatureId);
  // parent is attribute (widget)
  if (!myCurrentWidget.empty()) {
    std::string aParentId = restoreAttribute(myCurrentWidget.c_str(), _ID);
    aMessage->setAttributeId(aParentId);
  }
  aEvLoop->send(aMessage);
}

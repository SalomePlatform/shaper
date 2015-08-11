// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_WidgetAPI.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: sbh
 */

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <Config_Common.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>

Config_WidgetAPI::Config_WidgetAPI(const std::string& theRawXml)
{
  myDoc = xmlParseDoc(BAD_CAST theRawXml.c_str());
  myCurrentNode = xmlDocGetRootElement(myDoc);
}

Config_WidgetAPI::~Config_WidgetAPI()
{
  xmlFreeDoc(myDoc);
}

bool Config_WidgetAPI::toNextWidget()
{
  //Skip all non-element node, stop if next node is null
  xmlNodePtr aNextNode = myCurrentNode;
  do {
    aNextNode = aNextNode->next;
  } while (aNextNode && !isElementNode(aNextNode));

  if (!aNextNode) {
    toParentWidget();
    return false;
  }
  myCurrentNode = aNextNode;
  return true;
}

bool Config_WidgetAPI::toChildWidget()
{
  if (myCurrentNode && hasChild(myCurrentNode)) {
    myCurrentNode = myCurrentNode->children;
    while (myCurrentNode && !isElementNode(myCurrentNode)) {
      myCurrentNode = myCurrentNode->next;
    }
    return myCurrentNode != NULL;
  }
  return false;
}

bool Config_WidgetAPI::toParentWidget()
{
  if (myCurrentNode) {
    myCurrentNode = myCurrentNode->parent;
  }
  return myCurrentNode != NULL;
}

std::list<xmlNodePtr> Config_WidgetAPI::attributes() const
{
  std::list<xmlNodePtr> aResult;
  if (myCurrentNode && hasChild(myCurrentNode)) {
    xmlNodePtr anAttributesNode = myCurrentNode->children;
    while (anAttributesNode &&
           (!isElementNode(anAttributesNode) ||
            std::string((char *) anAttributesNode->name) != NODE_NAME_ATTRIBUTES)) {
      anAttributesNode = anAttributesNode->next;
    }
    if (anAttributesNode && hasChild(anAttributesNode)) {
      xmlNodePtr anAttributeNode = anAttributesNode->children;
      while (anAttributeNode) {
        if (isElementNode(anAttributeNode) &&
            std::string((char *) anAttributeNode->name) == NODE_NAME_ATTRIBUTE)
          aResult.push_back(anAttributeNode);
        anAttributeNode = anAttributeNode->next;
      }
    }
  }
  return aResult;
}

std::string Config_WidgetAPI::widgetType() const
{
  std::string result = "";
  if (myCurrentNode) {
    result = std::string((char *) myCurrentNode->name);
  }
  return result;
}

bool Config_WidgetAPI::isGroupBoxWidget() const
{
  return isNode(myCurrentNode, WDG_GROUP, WDG_CHECK_GROUP,
                NULL);
}

bool Config_WidgetAPI::isPagedWidget() const
{
  return isNode(myCurrentNode, WDG_TOOLBOX, WDG_SWITCH,
                NULL);
}

std::string Config_WidgetAPI::getProperty(const char* thePropName) const
{
  return ::getProperty(myCurrentNode, thePropName);
}

bool Config_WidgetAPI::getBooleanAttribute(const char* theAttributeName, bool theDefault) const
{
  return ::getBooleanAttribute(myCurrentNode, theAttributeName, theDefault);
}

std::string Config_WidgetAPI::widgetId() const
{
  return getProperty(_ID);
}

std::string Config_WidgetAPI::widgetIcon() const
{
  return getProperty(ATTR_ICON);
}

std::string Config_WidgetAPI::widgetLabel() const
{
  return getProperty(ATTR_LABEL);
}

std::string Config_WidgetAPI::widgetTooltip() const
{
  return getProperty(ATTR_TOOLTIP);
}

std::list<std::string> Config_WidgetAPI::getAttributes(const std::string& theRole/* = std::string()*/) const
{
  std::list<std::string> aResult;

  if (theRole.empty() || theRole == ATTR_MAIN_ROLE)
    aResult.push_back(widgetId());

  if (theRole == ATTR_MAIN_ROLE)
    return aResult;

  std::list<xmlNodePtr> anAttributes = attributes();
  for (auto it = anAttributes.begin(); it != anAttributes.end(); ++it) {
    if (theRole.empty() || theRole == ::getProperty(*it, ATTR_ROLE))
      aResult.push_back(::getProperty(*it, ATTR_ID));
  }
  return aResult;
}

std::string Config_WidgetAPI::getAttributeProperty(const std::string& theAttribute,
                                                   const std::string& thePropName) const
{
  if (theAttribute == widgetId()) {
    if (thePropName == ATTR_ROLE)
      return ATTR_MAIN_ROLE;
    return ::getProperty(myCurrentNode, thePropName.c_str());
  }

  std::list<xmlNodePtr> anAttributes = attributes();
  for (auto it = anAttributes.begin(); it != anAttributes.end(); ++it) {
    if (theAttribute == ::getProperty(*it, ATTR_ID))
      return ::getProperty(*it, thePropName.c_str());
  }
  return std::string();
}


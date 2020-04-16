// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <Config_Common.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>

Config_WidgetAPI::Config_WidgetAPI(std::string theRawXml, const std::string theAttributePrefix)
{
  myDoc = xmlParseDoc(BAD_CAST theRawXml.c_str());
  myCurrentNode = xmlDocGetRootElement(myDoc);
  myFeatureId = getProperty(_ID);
  myAttributePrefix = theAttributePrefix;
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
    xmlNodePtr aChildNode = myCurrentNode->children;
    // it is possible that among child nodes, there is no an element node, so
    // we should not change the current node until not-zero node is found
    // otherwise, it may happens that the current node is null and the node tree information
    // is lost
    while (aChildNode && !isElementNode(aChildNode)) {
      aChildNode = aChildNode->next;
    }
    if (aChildNode != NULL) {
      myCurrentNode = aChildNode;
      return true;
    }
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
  return isNode(myCurrentNode, WDG_GROUP, WDG_OPTIONALBOX,
                NULL);
}

bool Config_WidgetAPI::isPagedWidget() const
{
  return isNode(myCurrentNode, WDG_TOOLBOX, WDG_SWITCH, WDG_RADIOBOX,
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

std::string Config_WidgetAPI::featureId() const
{
  return myFeatureId;
}

std::string Config_WidgetAPI::widgetId() const
{
  return myAttributePrefix + getProperty(_ID);
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

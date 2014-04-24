/*
 * Config_WidgetReader.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include <Config_WidgetReader.h>
#include <Config_Keywords.h>
#include <Config_Common.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlstring.h>

#ifdef _DEBUG
#include <iostream>
#endif


Config_WidgetReader::Config_WidgetReader(const std::string& theXmlFile)
    : Config_XMLReader(theXmlFile)

{
}

Config_WidgetReader::~Config_WidgetReader()
{
}

std::string Config_WidgetReader::featureWidgetCfg(const std::string& theFeatureName)
{
  return myWidgetCache[theFeatureName];
}

std::string Config_WidgetReader::featureDescription(const std::string& theFeatureName)
{
  return myDescriptionCache[theFeatureName];
}


void Config_WidgetReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    std::string aNodeName = getProperty(theNode, _ID);
    myWidgetCache[aNodeName] = dumpNode(theNode);;
    myDescriptionCache[aNodeName] = getProperty(theNode, FEATURE_TEXT);
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

bool Config_WidgetReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
}

std::string Config_WidgetReader::dumpNode(xmlNodePtr theNode)
{
  std::string result = "";
  if (!hasChild(theNode)) {
    return result;
  }
  xmlNodePtr aChildrenNode = xmlFirstElementChild(theNode);
  xmlBufferPtr buffer = xmlBufferCreate();
  if (isNode(aChildrenNode, NODE_SOURCE, NULL)) {
    Config_XMLReader aSourceReader = 
      Config_XMLReader(getProperty(aChildrenNode, SOURCE_FILE));
    xmlNodePtr aSourceRoot = aSourceReader.findRoot();
    int size = xmlNodeDump(buffer, aSourceRoot->doc, aSourceRoot, 0, 1);
  } else {
    int size = xmlNodeDump(buffer, theNode->doc, theNode, 0, 1);
  }
  result = std::string((char*) (buffer->content));
  return result;
}

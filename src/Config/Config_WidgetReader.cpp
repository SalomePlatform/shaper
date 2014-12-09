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

#include <list>

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
    myCurrentFeature = getProperty(theNode, _ID);
    myWidgetCache[myCurrentFeature] = dumpNode(theNode);
    myDescriptionCache[myCurrentFeature] = getProperty(theNode, FEATURE_TEXT);
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

bool Config_WidgetReader::processChildren(xmlNodePtr theNode)
{
  //Read all nodes recursively, source and validator nodes have no children
  return !isNode(theNode, NODE_VALIDATOR,
                          NODE_SELFILTER,
                          NODE_SOURCE, NULL);
}

void Config_WidgetReader::resolveSourceNodes(xmlNodePtr theNode)
{
  xmlNodePtr aNode = xmlFirstElementChild(theNode);
  std::list<xmlNodePtr> aSourceNodes;
  while (aNode != NULL) {
    if (isNode(aNode, NODE_SOURCE, NULL)) {
      Config_XMLReader aSourceReader = Config_XMLReader(getProperty(aNode, SOURCE_FILE));
      xmlNodePtr aSourceRoot = aSourceReader.findRoot();
      if (!aSourceRoot) {
        continue;
      }
      xmlNodePtr aSourceNode = xmlFirstElementChild(aSourceRoot);
      xmlNodePtr aTargetNode = xmlDocCopyNodeList(aNode->doc, aSourceNode);
      while (aTargetNode != NULL) {
        xmlNodePtr aNextNode = xmlNextElementSibling(aTargetNode);
        xmlAddPrevSibling(aNode, aTargetNode);
        aTargetNode = aNextNode;
      }
      aSourceNodes.push_back(aNode);
    }
    aNode = xmlNextElementSibling(aNode);
  }
  //Remove "SOURCE" node.
  std::list<xmlNodePtr>::iterator it = aSourceNodes.begin();
  for (; it != aSourceNodes.end(); it++) {
    xmlUnlinkNode(*it);
    xmlFreeNode(*it);
  }
}

std::string Config_WidgetReader::dumpNode(xmlNodePtr theNode)
{
  std::string result = "";
  if (!hasChild(theNode)) {
    return result;
  }
  //Replace all "source" nodes with content;
  resolveSourceNodes(theNode);
  xmlBufferPtr buffer = xmlBufferCreate();
  int size = xmlNodeDump(buffer, theNode->doc, theNode, 0, 0);
  result = std::string((char*) (buffer->content));
  xmlBufferFree(buffer);
  return result;
}

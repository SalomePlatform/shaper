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
    std::string aFeature = getProperty(theNode, _ID);
    myWidgetCache[aFeature] = dumpNode(theNode);
    myDescriptionCache[aFeature] = getProperty(theNode, FEATURE_TEXT);
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

bool Config_WidgetReader::processChildren(xmlNodePtr theNode)
{
  //Read all nodes recursively, source and validator nodes have no children
  return !isNode(theNode, NODE_VALIDATOR,
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
      if (aSourceRoot) {
        xmlNodePtr aSourceNode = xmlFirstElementChild(aSourceRoot);
        xmlNodePtr aTargetNode = xmlDocCopyNodeList(aNode->doc, aSourceNode);
        while (aTargetNode != NULL) {
          xmlNodePtr aNextNode = xmlNextElementSibling(aTargetNode);
          xmlAddPrevSibling(aNode, aTargetNode);
          aTargetNode = aNextNode;
        }
        aSourceNodes.push_back(aNode);
      }
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
    // feature which has the next property should be dumped itself
    std::string anOwnPanel = getProperty(theNode, PROPERTY_PANEL_ID);
    if (!anOwnPanel.empty()) {
      xmlBufferPtr buffer = xmlBufferCreate();
#ifdef _DEBUG
      int size =
#endif
        xmlNodeDump(buffer, theNode->doc, theNode, 0, 0);
      result = std::string((char*) (buffer->content));
      xmlBufferFree(buffer);
    }
    return result;
  }
  //Replace all "source" nodes with content;
  resolveSourceNodes(theNode);
  xmlBufferPtr buffer = xmlBufferCreate();
#ifdef _DEBUG
  int size =
#endif
    xmlNodeDump(buffer, theNode->doc, theNode, 0, 0);
  result = std::string((char*) (buffer->content));
  xmlBufferFree(buffer);
  return result;
}

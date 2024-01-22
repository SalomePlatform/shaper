// Copyright (C) 2014-2024  CEA, EDF
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

#include "Config_Common.h"
#include <Config_Keywords.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <sstream> // for stringstream

#include <string>
#include <algorithm> // for std::transform
#include <vector>

bool isElementNode(xmlNodePtr theNode)
{
  if (!theNode)
    return false;
  return theNode->type == XML_ELEMENT_NODE;
}

bool isNode(xmlNodePtr theNode, const char* theNodeName, ...)
{
  const xmlChar* aName = theNode->name;
  if (!aName || !isElementNode(theNode)) {
    return false;
  }
  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName)) {
    return true;
  }
  va_list args;  // define argument list variable
  va_start(args, theNodeName);  // init list; point to last defined argument
  while (true) {
    char *anArg = va_arg (args, char*);  // get next argument
    if (anArg == NULL)
      break;
    if (!xmlStrcmp(aName, (const xmlChar *) anArg)) {
      va_end(args);  // cleanup the system stack
      return true;
    }
  }
  va_end(args);  // cleanup the system stack
  return false;
}

bool isAttributeNode(xmlNodePtr theNode)
{
  if(!isElementNode(theNode))
    return false;
  // it's parent is "feature" or "source" or page ("case" or "box")
  if(!hasParent(theNode, NODE_FEATURE, NODE_SOURCE,
                         WDG_GROUP, WDG_OPTIONALBOX,
                         WDG_TOOLBOX_BOX, WDG_RADIOBOX_ITEM,
                         WDG_SWITCH_CASE, NULL))
    return false;

  //it should not be a "source" or a "validator" node
  bool isLogical = isNode(theNode, NODE_SOURCE, NODE_VALIDATOR, NULL);
  // here must be only widgets not connected to attributes
  bool isPagedContainer = isNode(theNode, WDG_TOOLBOX_BOX,
                                          WDG_GROUP,
                                          WDG_SWITCH_CASE,  NULL);
  return !isLogical && !isPagedContainer;
}

bool isWidgetNode(xmlNodePtr theNode)
{
  if(!isElementNode(theNode))
    return false;
  // it's parent is "feature" or "source" or a page ("box", "case")
  if(!hasParent(theNode, NODE_FEATURE, NODE_SOURCE, WDG_GROUP, WDG_OPTIONALBOX,
    WDG_TOOLBOX_BOX, WDG_RADIOBOX_ITEM, WDG_SWITCH_CASE, WDG_FILTER, NULL))
    return false;

  //it should not be a "source" or a "validator" node
  return !isNode(theNode, NODE_SOURCE, NODE_VALIDATOR, NULL);
}

// LCOV_EXCL_START
// widget api?
bool isCaseNode(xmlNodePtr theNode)
{
  if(!isElementNode(theNode))
    return false;

  return isNode(theNode, WDG_OPTIONALBOX, WDG_SWITCH_CASE, WDG_TOOLBOX_BOX,
    WDG_RADIOBOX_ITEM, NULL);
}
// LCOV_EXCL_STOP

bool hasChild(xmlNodePtr theNode)
{
  xmlNodePtr aNode = theNode->children;
  for (; aNode; aNode = aNode->next) {
    if (isElementNode(theNode)) {
      return true;
    }
  }
  return false;
}

bool hasParent(xmlNodePtr theNode)
{
  xmlNodePtr aNode = theNode->parent;
  if (!aNode) {
    return false;
  }
  for (; aNode; aNode = aNode->next) {
    if (isElementNode(theNode)) {
      return true;
    }
  }
  return false;
}

bool hasParent(xmlNodePtr theNode, const char* theNodeName, ...)
{
  if (!hasParent(theNode)) {
    return false; // have no parents at all
  }
  xmlNodePtr aNode = theNode->parent;
  const xmlChar* aName = aNode->name;
  if (!aName || !isElementNode(aNode)) {
    return false;
  }
  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName)) {
    return true;
  }
  va_list args;  // define argument list variable
  va_start(args, theNodeName);  // init list; point to last defined argument
  while (true) {
    char *anArg = va_arg (args, char*);  // get next argument
    if (anArg == NULL)
      break;
    if (!xmlStrcmp(aName, (const xmlChar *) anArg)) {
      va_end(args);  // cleanup the system stack
      return true;
    }
  }
  va_end(args);  // cleanup the system stack
  return false;
}

xmlNodePtr hasParentRecursive(xmlNodePtr theNode, const std::vector<const char*>& theNodeNames)
{
  if (!hasParent(theNode)) {
    return 0; // have no parents at all
  }
  xmlNodePtr aNode = theNode->parent;
  const xmlChar* aName = aNode->name;
  if (!aName || !isElementNode(aNode)) {
    return 0;
  }
  for (size_t anIndex = 0; anIndex < theNodeNames.size(); ++anIndex) {
    if (!xmlStrcmp(aName, (const xmlChar *) theNodeNames[anIndex]))
      return aNode;
  }
  return hasParentRecursive(aNode, theNodeNames);
}

xmlNodePtr hasParentRecursive(xmlNodePtr theNode, const char* theNodeName, ...)
{
  std::vector<const char*> aNodeNames;
  va_list args;  // define argument list variable
  va_start(args, theNodeName);  // init list; point to last defined argument
  aNodeNames.push_back(theNodeName);
  while (true) {
    char *anArg = va_arg (args, char*);  // get next argument
    if (anArg == NULL)
      break;
    aNodeNames.push_back(anArg);
  }
  va_end(args);  // cleanup the system stack
  return hasParentRecursive(theNode, aNodeNames);
}

bool getParametersInfo(xmlNodePtr theNode, std::string& outPropertyId,
                      std::list<std::string>& outValidatorParameters)
{
  //Property id:
  char* anIdProp = (char*) xmlGetProp(theNode, BAD_CAST _ID);
  if (!anIdProp || anIdProp[0] == 0) {
    return false;
  }
  outPropertyId = std::string(anIdProp);

  //Property parameters:
  char* aParamProp = (char*) xmlGetProp(theNode, BAD_CAST _PARAMETERS);
  if (aParamProp && aParamProp[0] != 0) {
    std::string aPropString = std::string(aParamProp);
    std::stringstream aPropStringStream(aPropString);
    char COMMA_DELIM = ',';
    std::string aParameter;
    while (std::getline(aPropStringStream, aParameter, COMMA_DELIM)) {
      outValidatorParameters.push_back(aParameter);
    }
  }
  return true;
}

std::string library(const std::string& theLibName)
{
  if(theLibName.empty())
    return std::string();
  std::string aLibName = theLibName;
#ifndef WIN32
  static std::string aLibExt( ".so" );
  if (aLibName.size() < 3 || aLibName.substr(0, 3) !="lib") {
    aLibName = "lib" + aLibName;
  }
#else
  static std::string aLibExt(".dll");
#endif
  std::string anExt = aLibName.substr(aLibName.size() - 4);
  if (anExt != aLibExt)
    aLibName += aLibExt;

  return aLibName;
}

bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

std::string getProperty(xmlNodePtr theNode, const char* thePropName)
{
  std::string result = "";
  xmlChar* aPropChars = xmlGetProp(theNode, BAD_CAST thePropName);
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string((char*)aPropChars);
  xmlFree(aPropChars);

  std::string::iterator new_end = std::unique(result.begin(), result.end(), BothAreSpaces);
  result.erase(new_end, result.end());

  return result;
}

// LCOV_EXCL_START
std::string getContent(xmlNodePtr theNode)
{
  std::string result = "";
  xmlChar* aContent = xmlNodeGetContent(theNode);
  if (!aContent || aContent[0] == 0)
    return result;
  result = std::string((char*)aContent);
  xmlFree(aContent);
  return result;
}
// LCOV_EXCL_STOP

std::string getNormalizedProperty(xmlNodePtr theNode, const char* thePropName)
{
  return normalize(getProperty(theNode, thePropName));
}

bool getBooleanAttribute(xmlNodePtr theNode, const char* theAttributeName, bool theDefault)
{
  std::string prop = normalize(getProperty(theNode, theAttributeName));
  bool result = theDefault;
  if (prop == "true" || prop == "1") {
    result = true;
  } else if (prop == "false" || prop == "0") {
    result = false;
  }
  return result;
}

CONFIG_EXPORT std::string normalize(const std::string& theString)
{
  std::string result = theString;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](char c) { return static_cast<char>(::tolower(c)); });
  return result;
}

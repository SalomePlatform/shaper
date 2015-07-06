// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_Common.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: sbh
 */

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
                         WDG_GROUP, WDG_CHECK_GROUP,
                         WDG_TOOLBOX_BOX, WDG_SWITCH_CASE, NULL))
    return false;

  //it should not be a "source" or a "validator" node
  bool isLogical = isNode(theNode, NODE_SOURCE, NODE_VALIDATOR, NODE_SELFILTER, NULL);
  bool isPagedContainer = isNode(theNode, WDG_TOOLBOX, WDG_TOOLBOX_BOX,
                                          WDG_GROUP, WDG_CHECK_GROUP,
                                          WDG_SWITCH, WDG_SWITCH_CASE,  NULL);
  return !isLogical && !isPagedContainer;
}

bool isWidgetNode(xmlNodePtr theNode)
{
  if(!isElementNode(theNode))
    return false;
  // it's parent is "feature" or "source" or a page ("box", "case")
  if(!hasParent(theNode, NODE_FEATURE, NODE_SOURCE, WDG_GROUP,
                         WDG_TOOLBOX_BOX, WDG_SWITCH_CASE, NULL))
    return false;

  //it should not be a "source" or a "validator" node
  return !isNode(theNode, NODE_SOURCE, NODE_VALIDATOR, NODE_SELFILTER, NULL);
}

// widget api?
bool isCaseNode(xmlNodePtr theNode)
{
  if(!isElementNode(theNode))
    return false;

  return isNode(theNode, WDG_SWITCH_CASE, WDG_TOOLBOX_BOX, NULL);
}

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

bool hasParentRecursive(xmlNodePtr theNode, const std::vector<const char*>& theNodeNames)
{
  if (!hasParent(theNode)) {
    return false; // have no parents at all
  }
  xmlNodePtr aNode = theNode->parent;
  const xmlChar* aName = aNode->name;
  if (!aName || !isElementNode(aNode)) {
    return false;
  }
  for (size_t anIndex = 0; anIndex < theNodeNames.size(); ++anIndex) {
    if (!xmlStrcmp(aName, (const xmlChar *) theNodeNames[anIndex]))
      return true;
  }
  return hasParentRecursive(aNode, theNodeNames);
}

bool hasParentRecursive(xmlNodePtr theNode, const char* theNodeName, ...)
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
    while (std::getline(aPropStringStream, aParameter, ',')) {
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

std::string getProperty(xmlNodePtr theNode, const char* thePropName)
{
  std::string result = "";
  char* aPropChars = (char*) xmlGetProp(theNode, BAD_CAST thePropName);
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}

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

CONFIG_EXPORT std::string normalize(const char* theString)
{
  if (!theString)
    return std::string();
  return normalize(std::string(theString));
}

CONFIG_EXPORT std::string normalize(const std::string& theString)
{
  std::string result = theString;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

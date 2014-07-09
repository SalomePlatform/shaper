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

#include <sstream> //for stringstream

bool isElementNode(xmlNodePtr theNode)
{
  return theNode->type == XML_ELEMENT_NODE;
}

bool isNode(xmlNodePtr theNode, const char* theNodeName, ...)
{
  bool result = false;
  const xmlChar* aName = theNode->name;
  if (!aName || !isElementNode(theNode)) {
    return false;
  }
  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName)) {
    return true;
  }
  va_list args; // define argument list variable
  va_start(args, theNodeName); // init list; point to last defined argument
  while(true) {
    char *anArg = va_arg (args, char*); // get next argument
    if (anArg == NULL)
      break;
    if (!xmlStrcmp(aName, (const xmlChar *) anArg)) {
      va_end(args); // cleanup the system stack
      return true;
    }
  }
  va_end(args); // cleanup the system stack
  return false;
}

bool hasChild(xmlNodePtr theNode)
{
  xmlNodePtr aNode = theNode->children;
  for( ; aNode; aNode = aNode->next) {
    if (isElementNode(theNode)) {
      return true;
    }
  }
  return false;
}

bool getValidatorInfo(xmlNodePtr theNode,
                      std::string& outValidatorId,
                      std::list<std::string>& outValidatorParameters)
{
  //Validator id:
  char* anIdProp = (char*) xmlGetProp(theNode, BAD_CAST _ID);
  if (!anIdProp || anIdProp[0] == 0) {
    return false;
  }
  outValidatorId = std::string(anIdProp);

  //Validator parameters:
  char* aParamProp = (char*) xmlGetProp(theNode, BAD_CAST VALIDATOR_PARAMETERS);
  if (aParamProp && aParamProp[0] != 0) {
    std::string aPropString = std::string(aParamProp);
    std::stringstream aPropStringStream(aPropString);
    char COMMA_DELIM = ',';
    std::string aValidatorParameter;
    while (std::getline(aPropStringStream, aValidatorParameter, ',')) {
      outValidatorParameters.push_back(aValidatorParameter);
    }
  }
  return true;
}

std::string library(const std::string& theLibName)
{
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

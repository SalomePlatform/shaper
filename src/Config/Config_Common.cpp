/*
 * Config_Common.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: sbh
 */

#include "Config_Common.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

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

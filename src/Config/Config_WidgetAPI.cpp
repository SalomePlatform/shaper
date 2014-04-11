/*
 * Config_WidgetAPI.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: sbh
 */

#include <Config_WidgetAPI.h>

#include <libxml/parser.h>
#include <libxml/tree.h>


Config_WidgetAPI::Config_WidgetAPI(std::string theRawXml)
{
  myDoc = xmlParseDoc(BAD_CAST theRawXml.c_str());
  myCurrentNode = NULL;
}


Config_WidgetAPI::~Config_WidgetAPI()
{
  xmlFreeDoc(myDoc);
}

void Config_WidgetAPI::reset()
{
  xmlNodePtr aRoot = xmlDocGetRootElement(myDoc);
  if(aRoot) {
    myCurrentNode = aRoot->children;
  }
}

bool Config_WidgetAPI::nextWidget()
{
  if(myCurrentNode) {
    myCurrentNode = myCurrentNode->next;
  }
  return myCurrentNode != NULL;
}

std::string Config_WidgetAPI::widgetType()
{
  std::string result = "";
  if(myCurrentNode) {
    result = std::string((char *) myCurrentNode->name);
  }
  return result;
}

std::string Config_WidgetAPI::getProperty(const char* thePropName)
{
  std::string result = "";
  char* aPropChars = (char*) xmlGetProp(myCurrentNode, BAD_CAST thePropName);
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}

std::string Config_WidgetAPI::widgetId()
{
  return getProperty("id");
}

std::string Config_WidgetAPI::widgetTooltip()
{
  return getProperty("tooltip");
}

std::string Config_WidgetAPI::widgetIcon()
{
  return getProperty("icon");
}

std::string Config_WidgetAPI::widgetLabel()
{
  return getProperty("label");
}

bool Config_WidgetAPI::isNode(xmlNodePtr theNode, const char* theNodeName, ...)
{
  bool result = false;
  const xmlChar* aName = theNode->name;
  if (!aName || theNode->type != XML_ELEMENT_NODE)
    return false;

  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName))
    return true;

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

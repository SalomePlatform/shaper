/*
 * Config_XMLReader.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#include <Config_XMLReader.h>

#include <Event_Loop.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

/*
#ifdef WIN32
//For GetModuleFileNameW
#include <windows.h>
#endif
*/

#ifdef _DEBUG
#include <iostream>
#endif

Config_XMLReader::Config_XMLReader(const std::string& theXmlFileName)
    : myXmlDoc(NULL)
{
  std::string prefix = "";
  /*
   * Get path to *.xml files (typically ./bin/../plugins/)

   * the problem: application may be launched using python executable,
   * to use environment variable (at least for the current moment)
   */
  char* anEnv = getenv("NEW_GEOM_CONFIG_FILE");
  if (anEnv) {
    prefix = std::string(anEnv) + "/";
  }

  myDocumentPath = prefix + theXmlFileName;
}

Config_XMLReader::~Config_XMLReader()
{
  xmlFreeDoc(myXmlDoc);
}

/*
 * Read all nodes in associated xml file,
 * recursively if processChildren(xmlNode) is true for the xmlNode.
 * For each read node the processNode will be called.
 */
void Config_XMLReader::readAll()
{
  xmlNodePtr aRoot = findRoot();
  readRecursively(aRoot);
}

/*
 * Allows to customize reader's behavior for a node. Virtual.
 * The default implementation does nothing. (In debug mode prints
 * some info)
 */
void Config_XMLReader::processNode(xmlNodePtr aNode)
{
#ifdef _DEBUG
  std::cout << "Config_XMLReader::processNode: "
  << aNode->name << " content: "
  << aNode->content << std::endl;
#endif
}

/*
 * Defines which nodes should be processed recursively. Virtual.
 * The default implementation is to read all nodes.
 */
bool Config_XMLReader::processChildren(xmlNodePtr aNode)
{
  return true;
}

/*
 *
 */
xmlNodePtr Config_XMLReader::findRoot()
{
  myXmlDoc = xmlParseFile(myDocumentPath.c_str());
  if (myXmlDoc == NULL) {
#ifdef _DEBUG
    std::cout << "Config_XMLReader::import: " << "Document " << myDocumentPath
    << " is not parsed successfully." << std::endl;
#endif
    return NULL;
  }

  xmlNodePtr aRoot = xmlDocGetRootElement(myXmlDoc);
#ifdef _DEBUG
  if(aRoot == NULL) {
    std::cout << "Config_XMLReader::import: " << "Error: empty document";
  }
#endif
  return aRoot;
}

/*
 * Calls processNode() for each child (for some - recursively)
 * of the given node.
 * \sa ReadAll()
 */
void Config_XMLReader::readRecursively(xmlNodePtr theParent)
{
  if (!theParent)
    return;
  xmlNodePtr aNode = theParent->xmlChildrenNode;
  for(; aNode; aNode = aNode->next) {
    processNode(aNode);
    if (processChildren(aNode)) {
      readRecursively(aNode);
    }
  }
}

/*
 * void* -> xmlNodePtr
 */
xmlNodePtr Config_XMLReader::node(void* theNode)
{
  return static_cast<xmlNodePtr>(theNode);
}

/*
 * Returns named property for a given node as std::string.
 */
std::string Config_XMLReader::getProperty(xmlNodePtr theNode, const char* name)
{
  std::string result = "";
  char* aPropChars = (char*) xmlGetProp(theNode, BAD_CAST name);
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}


/*
 * Config_XMLReader.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#include <Config_XMLReader.h>
#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_ValidatorMessage.h>
#include <Config_PropManager.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <fstream>

#ifdef WIN32
#pragma warning(disable : 4996) // for getenv
#endif

#ifdef _DEBUG
#include <iostream>
#endif

Config_XMLReader::Config_XMLReader(const std::string& theXmlFileName)
    : myXmlDoc(NULL)
{
  std::string prefix = ""; 
  Config_Prop* aProp = Config_PropManager::findProp("Plugins", "default_path");
  if (aProp)
    prefix = aProp->value();
  /*
   * Get path to *.xml files (typically ./bin/../plugins/)

   * the problem: application may be launched using python executable,
   * to use environment variable (at least for the current moment)
   */
  if (prefix.empty()) {
    char* anEnv = getenv("NEW_GEOM_CONFIG_FILE");
    if (anEnv) {
      prefix = std::string(anEnv);
    }
  }
#ifdef WIN32
    prefix += "\\";
#else
    prefix += "/";
#endif
  myDocumentPath = prefix + theXmlFileName;
  std::ifstream aTestFile(myDocumentPath);
  if (!aTestFile) Events_Error::send("Unable to open " + myDocumentPath);
  aTestFile.close();
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
 * The default impl does nothing. (In debug mode prints
 * some info)
 */
void Config_XMLReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_SOURCE, NULL)) {
    std::string aSourceFile = getProperty(theNode, SOURCE_FILE);
    Config_XMLReader aSourceReader = Config_XMLReader(aSourceFile);
    readRecursively(aSourceReader.findRoot());
#ifdef _DEBUG
    std::cout << "Config_XMLReader::sourced node: " << aSourceFile << std::endl;
#endif
  } else if (isNode(theNode, NODE_VALIDATOR, NULL)) {
    processValidator(theNode);
  }
}

/*
 * Defines which nodes should be processed recursively. Virtual.
 * The default impl is to read all nodes.
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
  if (myXmlDoc == NULL) {
    myXmlDoc = xmlParseFile(myDocumentPath.c_str());
  }
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
  for (; aNode; aNode = aNode->next) {
    //Still no text processing in features...
    if (!isElementNode(aNode)) {
      continue;
    }
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

std::string Config_XMLReader::getNodeName(xmlNodePtr theNode)
{
  std::string result = "";
  char* aPropChars = (char*) theNode->name;
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}

void Config_XMLReader::processValidator(xmlNodePtr theNode)
{
  Events_ID aValidatoEvent = Events_Loop::eventByName(EVENT_VALIDATOR_LOADED);
  Events_Loop* aEvLoop = Events_Loop::loop();
  std::shared_ptr<Config_ValidatorMessage> 
    aMessage(new Config_ValidatorMessage(aValidatoEvent, this));
  std::string aValidatorId;
  std::list<std::string> aValidatorParameters;
  getValidatorInfo(theNode, aValidatorId, aValidatorParameters);
  aMessage->setValidatorId(aValidatorId);
  aMessage->setValidatorParameters(aValidatorParameters);
  xmlNodePtr aFeatureOrWdgNode = theNode->parent;
  if (isNode(aFeatureOrWdgNode, NODE_FEATURE, NULL)) {
    aMessage->setFeatureId(getProperty(aFeatureOrWdgNode, _ID));
  } else {
    aMessage->setAttributeId(getProperty(aFeatureOrWdgNode, _ID));
    aMessage->setFeatureId(myCurrentFeature);
  }
  aEvLoop->send(aMessage);
}

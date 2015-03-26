// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_XMLReader.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#include <Config_XMLReader.h>
#include <Config_Keywords.h>
#include <Config_Common.h>
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

void Config_XMLReader::readAll()
{
  xmlNodePtr aRoot = findRoot();
  readRecursively(aRoot);
}

void Config_XMLReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_SOURCE, NULL)) {
    std::string aSourceFile = getProperty(theNode, SOURCE_FILE);
    Config_XMLReader aSourceReader = Config_XMLReader(aSourceFile);
    readRecursively(aSourceReader.findRoot());
#ifdef _DEBUG
    //std::cout << "Config_XMLReader::sourced node: " << aSourceFile << std::endl;
#endif
  }
}

void Config_XMLReader::cleanup(xmlNodePtr)
{
  // do nothing;
}

bool Config_XMLReader::processChildren(xmlNodePtr aNode)
{
  return true;
}

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
    cleanup(aNode);
  }
}

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

void Config_XMLReader::storeAttribute(xmlNodePtr theNode, const char* theAttribute)
{
  std::string aKey = getNodeName(theNode) + ":" + std::string(theAttribute);
  std::string aValue = getProperty(theNode, theAttribute);
  if(!aValue.empty()) {
    myCachedAttributes[aKey] = aValue;
  }
}

std::string Config_XMLReader::restoreAttribute(xmlNodePtr theNode, const char* theAttribute)
{
  return restoreAttribute(getNodeName(theNode).c_str(), theAttribute);
}

std::string Config_XMLReader::restoreAttribute(const char* theNodeName, const char* theAttribute)
{
  std::string aKey = std::string(theNodeName) + ":" + std::string(theAttribute);
  std::string result = "";
  if(myCachedAttributes.find(aKey) != myCachedAttributes.end()) {
    result = myCachedAttributes[aKey];
  }
  return result;
}

bool Config_XMLReader::cleanupAttribute(xmlNodePtr theNode, const char* theNodeAttribute)
{
  return cleanupAttribute(getNodeName(theNode).c_str(), theNodeAttribute);
}

bool Config_XMLReader::cleanupAttribute(const char* theNodeName, const char* theNodeAttribute)
{
  std::string aKey = std::string(theNodeName) + ":" + std::string(theNodeAttribute);
  bool result = false;
  std::map<std::string, std::string>::iterator anEntry = myCachedAttributes.find(aKey);
  if( anEntry != myCachedAttributes.end()) {
    myCachedAttributes.erase(anEntry);
    result = true;
  }
  return result;
}

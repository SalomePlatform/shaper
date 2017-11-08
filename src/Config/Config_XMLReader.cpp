// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Config_XMLReader.h>
#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_PropManager.h>
#include <Config_ModuleReader.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <fstream>
#include <sstream>

#ifdef WIN32
#pragma warning(disable : 4996) // for getenv
#endif

#ifdef _DEBUG
#include <iostream>
#endif

#ifdef WIN32
    static const char FSEP = '\\';
#else
    static const char FSEP = '/';
#endif

Config_XMLReader::Config_XMLReader(const std::string& theXmlFileName)
    : myXmlDoc(NULL), myRootFileName(theXmlFileName)
{
  myDocumentPath = findConfigFile(theXmlFileName);
  if (myDocumentPath.empty()) {
    Events_InfoMessage("Config_XMLReader", "Unable to open %1").arg(theXmlFileName).send();
  }
}

Config_XMLReader::~Config_XMLReader()
{
  xmlFreeDoc(myXmlDoc);
}

std::string Config_XMLReader::resourcesConfigFile()
{
  std::string aValue;
  char* anEnv = getenv("SHAPER_ROOT_DIR");
  if (anEnv) {
    aValue = std::string(anEnv) +
      FSEP + "share" + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper";
  } else {
    anEnv = getenv("OPENPARTS_ROOT_DIR");
    if (anEnv) {
      aValue = std::string(anEnv) + FSEP + "resources";
    }
  }
  return aValue;
}

std::string Config_XMLReader::pluginConfigFile()
{
  std::string aValue;
  char* anEnv = getenv("SHAPER_ROOT_DIR");
  if (anEnv) {
    aValue = std::string(anEnv) +
      FSEP + "share" + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper";
  } else {
    anEnv = getenv("OPENPARTS_ROOT_DIR");
    if (anEnv) {
      aValue = std::string(anEnv) + FSEP + "plugins";
    }
  }
  return aValue;
}

std::string Config_XMLReader::findConfigFile(const std::string theFileName, const int theFindIndex)
{
  int aResultIndex = 0;
  for(int aSolution = 0; aSolution < 12; aSolution++) {
    std::string aFileName;
    if (aSolution == 0) {
      Config_Prop* aProp = Config_PropManager::findProp("Plugins", "default_path");
      if (!aProp)
        continue;
      aFileName = aProp->value();
    } else {
      std::ostringstream anEnvName;
      if (aSolution == 1)
        anEnvName<<"SHAPER_ROOT_DIR";
      else if (aSolution == 2)
        anEnvName<<"OPENPARTS_ROOT_DIR";
      else
        anEnvName<<"OPENPARTS_PLUGINS_DIR";

      char* anEnv = getenv(anEnvName.str().c_str());
      if (!anEnv)
        continue;
      if (aSolution > 2) { // there may be several paths separated by ";" symbol
        std::string anEnvPart = anEnv;
        size_t aPosStart = 0, aPosEnd;
        for(int aSubNum = 0; aSubNum < aSolution - 3; aSubNum++) {
          aPosStart++;
          aPosStart = anEnvPart.find(';', aPosStart);
          if (aPosStart == std::string::npos)
            break;
        }
        if (aPosStart == std::string::npos)
          break;
        if (aPosStart != 0)
          aPosStart++;
        aPosEnd = anEnvPart.find(';', aPosStart);
        aFileName = anEnvPart.substr(aPosStart,
          aPosEnd == std::string::npos ? aPosEnd : aPosEnd - aPosStart) + FSEP;
      } else {
        aFileName = std::string(anEnv) + FSEP;
      }
      if (aSolution == 1)
        aFileName += std::string("share") + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper";
      else if (aSolution == 2)
        aFileName += "plugins";
    }

    aFileName += FSEP + theFileName;
    std::ifstream aTestFile(aFileName);
    if (aTestFile) {
      if (aResultIndex == theFindIndex)
        return aFileName;
      aResultIndex++;
      if (aSolution == 1) // don't allow SHAPER and OpenParts paths treated simultaneously
        aSolution++;
    }
  }
  return ""; // no files found
}

void Config_XMLReader::readAll()
{
  // to load external modules dependencies (like GEOM for Connector Feature)
  Config_ModuleReader::loadScript("salome.shaper.initConfig", false);

  for(int aSolution = 0; true; aSolution++) {
    std::string aFoundFile = findConfigFile(myRootFileName, aSolution);
    if (aFoundFile.empty()) {
      break; // no more solutions
    }

    if (myXmlDoc != NULL) { // clear the previous XML document - now the new one will be opened
      xmlFreeDoc(myXmlDoc);
      myXmlDoc = NULL;
    }
    xmlNodePtr aRoot = findRoot(aFoundFile);
    readRecursively(aRoot);
  }
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

xmlNodePtr Config_XMLReader::findRoot(const std::string theDocumentPath)
{
  std::string aDocPath = theDocumentPath.empty() ? myDocumentPath : theDocumentPath;
  if (myXmlDoc == NULL) {
    myXmlDoc = xmlParseFile(aDocPath.c_str());
  }
  if (myXmlDoc == NULL) {
#ifdef _DEBUG
    std::cout << "Config_XMLReader::import: " << "Document " << aDocPath
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

void Config_XMLReader::storeAttribute(xmlNodePtr theNode, const char* theAttribute, bool doClean)
{
  std::string aKey = getNodeName(theNode) + ":" + std::string(theAttribute);
  std::string aValue = getProperty(theNode, theAttribute);
  if (doClean || !aValue.empty()) {
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

const char* Config_XMLReader::encoding() const
{
  return (const char*) myXmlDoc->encoding;
}
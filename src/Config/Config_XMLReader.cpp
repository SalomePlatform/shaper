/*
 * Config_XMLReader.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#include <Config_XMLReader.h>

#include <Event_Loop.hxx>

#include <libxml\parser.h>
#include <libxml\tree.h>
#ifdef WIN32
//For GetModuleFileNameW
#include <windows.h>
#endif

#ifdef _DEBUG
#include <iostream>
#endif

static bool IsNode(xmlNodePtr theNode, const char* theNodeName)
{
  return theNode->type == XML_ELEMENT_NODE
      && !xmlStrcmp(theNode->name, (const xmlChar *) theNodeName);
}

const static char* FEATURE_ID = "id";
const static char* FEATURE_TEXT = "text";
const static char* FEATURE_TOOLTIP = "tooltip";
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_KEYSEQUENCE = "keysequence";
const static char* FEATURE_GROUP_NAME = "name";

Config_XMLReader::Config_XMLReader(const std::string& theXmlFile)
{
  setDocumentPath(theXmlFile);
}

Config_XMLReader::~Config_XMLReader()
{
}

std::string Config_XMLReader::documentPath() const
{
  return m_DocumentPath;
}

void Config_XMLReader::setDocumentPath(std::string documentPath)
{
  std::string prefix;
#ifdef WIN32
  HMODULE hModule = GetModuleHandleW(NULL);
  WCHAR wchar_path[MAX_PATH];
  GetModuleFileNameW(hModule, wchar_path, MAX_PATH);
  char char_path[MAX_PATH];
  char DefChar = ' ';
  WideCharToMultiByte(CP_ACP, 0, wchar_path, -1, char_path, MAX_PATH, &DefChar, NULL);
  prefix = std::string(char_path);
  //chop "bin\XGUI.exe"
  unsigned found = prefix.rfind("bin");
  if(found != std::string::npos)
    prefix.replace(found, prefix.length(), "plugins\\");
#else
  //TODO(sbh): Find full path to binary on linux
  prefix = "../plugins/";
#endif
  m_DocumentPath = prefix + documentPath;
}

void Config_XMLReader::readAll()
{
  import();
}

/*
 * TODO: make virtual as beforeImport
 */
bool Config_XMLReader::import()
{
  bool result = false;
  xmlDocPtr aDoc;
  aDoc = xmlParseFile(m_DocumentPath.c_str());
  if(aDoc == NULL) {
    #ifdef _DEBUG
    std::cout << "Config_XMLReader::import: " << "Document " << m_DocumentPath
              << " is not parsed successfully." << std::endl;
    #endif
    return result;
  }
  xmlNodePtr aRoot = xmlDocGetRootElement(aDoc);
  if(aRoot == NULL) {
    #ifdef _DEBUG
    std::cout << "Config_XMLReader::import: " << "Error: empty document";
    #endif
    return result;
  }
  xmlNodePtr aWbSec;
  for(aWbSec = aRoot->xmlChildrenNode; aWbSec; aWbSec = aWbSec->next) { // searching for higher level element "workbench"
    if(IsNode(aWbSec, "workbench")) {
      result = importWorkbench(aWbSec);
    } else {
      #ifdef _DEBUG
      std::cout << "Config_XMLReader::import: "
                << "Found strange section, should be workbench" << std::endl;
      #endif
      continue;
    }
  }
  return result;
}

/*
 * TODO(sbh): make virtual as doImport
 */
bool Config_XMLReader::importWorkbench(void* theRoot)
{
  xmlNodePtr aGroupNode = (static_cast<xmlNodePtr>(theRoot))->xmlChildrenNode;
  Event_Loop* aEvLoop = Event_Loop::Loop();
  if(!aEvLoop) {
    #ifdef _DEBUG
    std::cout << "Config_XMLReader::importWorkbench: "
              << "No event loop registered" << std::endl;
    #endif
    return false;
  }
  for(; aGroupNode; aGroupNode = aGroupNode->next) { // searching for record
    if(!IsNode(aGroupNode, "group"))
      continue;
    std::string aGroupName = getProperty(aGroupNode, FEATURE_GROUP_NAME);
    if(aGroupName.empty())
      continue;
    xmlNodePtr aFtNode = aGroupNode->xmlChildrenNode;
    for(; aFtNode; aFtNode = aFtNode->next) {
      if(!IsNode(aFtNode, "feature"))
        continue;
      //Create feature...
      Config_FeatureMessage aMessage(aEvLoop->EventByName("Feature"), this);
      fillFeature(aFtNode, aMessage);
      aMessage.m_group = aGroupName;
      aEvLoop->Send(aMessage);
    }
  }
  return true;
}

void Config_XMLReader::fillFeature(void *theRoot,
                                   Config_FeatureMessage& outFeatureMessage)
{
  outFeatureMessage.m_id = getProperty(theRoot, FEATURE_ID);
  outFeatureMessage.m_text = getProperty(theRoot, FEATURE_TEXT);
  outFeatureMessage.m_tooltip = getProperty(theRoot, FEATURE_TOOLTIP);
  outFeatureMessage.m_icon = getProperty(theRoot, FEATURE_ICON);
  outFeatureMessage.m_keysequence = getProperty(theRoot, FEATURE_KEYSEQUENCE);
}

std::string Config_XMLReader::getProperty(void *theRoot, const char* name)
{
  std::string result = "";
  xmlNodePtr aNode = (static_cast<xmlNodePtr>(theRoot));
  char* aPropChars = (char*) xmlGetProp(aNode, BAD_CAST name);
  if(!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}

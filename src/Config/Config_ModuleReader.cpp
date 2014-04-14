/*
 * Config_ModuleReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef _DEBUG
#include <iostream>
#endif



Config_ModuleReader::Config_ModuleReader(const char* theEventGenerated)
    : Config_XMLReader("plugins.xml"), myIsAutoImport(false), myEventGenerated(theEventGenerated)
{
}

Config_ModuleReader::~Config_ModuleReader()
{
}

/*
 * Get module name from plugins.xml
 * (property "module")
 */
std::string Config_ModuleReader::getModuleName()
{
  xmlNodePtr aRoot = findRoot();
  return getProperty(aRoot, PLUGINS_MODULE);
}

/*
 *
 */
void Config_ModuleReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_PLUGIN, NULL)) {
    std::string aPluginConf = getProperty(theNode, PLUGIN_CONFIG);
    std::string aPluginLibrary = getProperty(theNode, PLUGIN_LIBRARY);
    if (myIsAutoImport)
      importPlugin(aPluginConf, aPluginLibrary);
    myPluginsMap[aPluginLibrary] = aPluginConf;
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

void Config_ModuleReader::importPlugin(const std::string& thePluginName,
                                       const std::string& thePluginLibrary)
{
  Config_FeatureReader* aReader;
  if(thePluginLibrary.empty()) {
    aReader = new Config_FeatureReader(thePluginName);
  } else {
    aReader = new Config_FeatureReader(thePluginName, thePluginLibrary, myEventGenerated);
  }
  aReader->readAll();
}

void Config_ModuleReader::setAutoImport(bool theEnabled)
{
  myIsAutoImport = theEnabled;
}

const std::map<std::string, std::string>& Config_ModuleReader::plugins() const
{
  return myPluginsMap;
}

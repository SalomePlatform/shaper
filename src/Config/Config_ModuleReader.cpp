/*
 * Config_ModuleReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>

#include <libxml\parser.h>
#include <libxml\tree.h>

#ifdef _DEBUG
#include <iostream>
#endif

//Hardcoded xml entities
// * Nodes
const static char* NODE_PLUGIN = "plugin";
const static char* NODE_PLUGINS = "plugins";

// * Properties
const static char* PLUGINS_MODULE = "module";
const static char* PLUGIN_CONFIG = "configuration";
const static char* PLUGIN_LIBRARY = "library";


Config_ModuleReader::Config_ModuleReader()
    : Config_XMLReader("plugins.xml"),
      m_isAutoImport(false)
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
  if(isNode(theNode, NODE_PLUGIN, NULL)) {
    std::string aPluginName = getProperty(theNode, PLUGIN_CONFIG);
    if(m_isAutoImport)
      importPlugin(aPluginName);
    m_pluginsList.push_back(aPluginName);
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

void Config_ModuleReader::importPlugin(const std::string& thePluginName)
{
  Config_FeatureReader aReader(thePluginName);
  aReader.readAll();
}

void Config_ModuleReader::setAutoImport(bool enabled)
{
  m_isAutoImport = enabled;
}

const std::list<std::string>& Config_ModuleReader::pluginsList() const
{
  return m_pluginsList;
}

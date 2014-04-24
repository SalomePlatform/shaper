/*
 * Config_ModuleReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef _DEBUG
#include <iostream>
#endif



Config_ModuleReader::Config_ModuleReader(const char* theEventGenerated)
    : Config_XMLReader("plugins.xml"),
      myEventGenerated(theEventGenerated)
{
}

Config_ModuleReader::~Config_ModuleReader()
{
}

const std::map<std::string, std::string>& Config_ModuleReader::featuresInFiles() const
{
  return myFeaturesInFiles;
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
    std::list<std::string> aFeatures = importPlugin(aPluginConf, aPluginLibrary);
    std::list<std::string>::iterator it = aFeatures.begin();
    for( ; it != aFeatures.end(); it++ ) {
      myFeaturesInFiles[*it] = aPluginConf;
    }
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

std::list<std::string>
Config_ModuleReader::importPlugin(const std::string& thePluginFile,
                                  const std::string& thePluginLibrary)
{
  Config_FeatureReader aReader = Config_FeatureReader(thePluginFile,
                                                      thePluginLibrary,
                                                      myEventGenerated);
  aReader.readAll();
  return aReader.features();
}


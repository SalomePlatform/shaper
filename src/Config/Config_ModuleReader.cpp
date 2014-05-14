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
#include <Events_Error.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

//Necessary for cerr
#include <iostream>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

Config_ModuleReader::Config_ModuleReader(const char* theEventGenerated)
    : Config_XMLReader("plugins.xml"), myEventGenerated(theEventGenerated)
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
    std::list<std::string> aFeatures = importPlugin(aPluginLibrary, aPluginConf);
    std::list<std::string>::iterator it = aFeatures.begin();
    for(; it != aFeatures.end(); it++) {
      myFeaturesInFiles[*it] = aPluginConf;
    }
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

std::list<std::string> Config_ModuleReader::importPlugin(const std::string& thePluginLibrary,
                                                         const std::string& thePluginFile)
{
  if (thePluginFile.empty()) { //probably a third party library
    loadLibrary(thePluginLibrary);
    return std::list<std::string>();
  }

  Config_FeatureReader aReader = Config_FeatureReader(thePluginFile, thePluginLibrary,
                                                      myEventGenerated);
  aReader.readAll();
  return aReader.features();
}

void Config_ModuleReader::loadLibrary(const std::string theLibName)
{
#ifdef _DEBUG
  std::cout << "Config_ModuleReader::loading library... "  << theLibName.c_str() << std::endl;
#endif
  std::string aFileName = library(theLibName);
  if (aFileName.empty())
    return;

#ifdef WIN32
  HINSTANCE aModLib = ::LoadLibrary(aFileName.c_str());
  if (!aModLib) {
    std::string errorMsg = "Failed to load " + aFileName;
    std::cerr << errorMsg << std::endl;
    Events_Error::send(errorMsg);
  }
#else
  void* aModLib = dlopen( aFileName.c_str(), RTLD_LAZY );
  if ( !aModLib ) {
    std::cerr << "Failed to load " << aFileName.c_str() << std::endl;
  }
#endif
}


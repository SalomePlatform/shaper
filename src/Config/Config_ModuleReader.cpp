// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_ModuleReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <pyconfig.h>

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>
#include <Events_Error.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

// Have to be included before std headers
#include <Python.h>

//Necessary for cerr
#include <iostream>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996) // for getenv
#else
#include <dlfcn.h>
#endif

std::map<std::string, Config_ModuleReader::PluginType> Config_ModuleReader::myPluginTypes;
std::set<std::string> Config_ModuleReader::myDependencyModules;

Config_ModuleReader::Config_ModuleReader(const char* theEventGenerated)
    : Config_XMLReader(PLUGIN_FILE),
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

const std::set<std::string>& Config_ModuleReader::modulePluginFiles() const
{
  return myPluginFiles;
}

/*!
 * Get module name from plugins.xml
 * (property "module")
 */
std::string Config_ModuleReader::getModuleName()
{
  xmlNodePtr aRoot = findRoot();
  return getProperty(aRoot, PLUGINS_MODULE);
}


void Config_ModuleReader::addFeature(const std::string& theFeatureName,
                                     const std::string& thePluginConfig)
{
  if (myFeaturesInFiles.count(theFeatureName)) {
    std::string anErrorMsg = "Can not register feature '" + theFeatureName + "' in plugin '"
        + thePluginConfig + "'. There is a feature with the same ID.";
    Events_Error::send(anErrorMsg);
    return;
  }

  myFeaturesInFiles[theFeatureName] = thePluginConfig;
}

void Config_ModuleReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_PLUGIN, NULL)) {
    if (!hasRequiredModules(theNode))
      return;
    std::string aPluginConf = getProperty(theNode, PLUGIN_CONFIG);
    if (!aPluginConf.empty())
      myPluginFiles.insert(aPluginConf);
    std::string aPluginLibrary = getProperty(theNode, PLUGIN_LIBRARY);
    std::string aPluginScript = getProperty(theNode, PLUGIN_SCRIPT);
    std::string aPluginName = addPlugin(aPluginLibrary, aPluginScript, aPluginConf);

    std::list<std::string> aFeatures = importPlugin(aPluginName, aPluginConf);
    std::list<std::string>::iterator it = aFeatures.begin();
    for (; it != aFeatures.end(); it++) {
      addFeature(*it, aPluginConf);
    }
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

bool Config_ModuleReader::hasRequiredModules(xmlNodePtr theNode) const
{
  std::string aRequiredModule = normalize(getProperty(theNode, PLUGIN_DEPENDENCY));
  if(aRequiredModule.empty())
    return true;
  std::set<std::string>::iterator it = myDependencyModules.begin();
  for ( ; it != myDependencyModules.end(); it++ ) {
    if (*it == aRequiredModule) return true;
  }
  return false;
}

std::list<std::string> Config_ModuleReader::importPlugin(const std::string& thePluginLibrary,
                                                         const std::string& thePluginXmlConf)
{
  if (thePluginXmlConf.empty()) {  //probably a third party library
    loadLibrary(thePluginLibrary);
    return std::list<std::string>();
  }

  Config_FeatureReader aReader = Config_FeatureReader(thePluginXmlConf,
                                                      thePluginLibrary,
                                                      myEventGenerated);
  aReader.readAll();
  return aReader.features();
}

std::string Config_ModuleReader::addPlugin(const std::string& aPluginLibrary,
                                           const std::string& aPluginScript,
                                           const std::string& aPluginConf)
{
  PluginType aType = Config_ModuleReader::Binary;
  std::string aPluginName;
  if (!aPluginLibrary.empty()) {
    aPluginName = aPluginLibrary;
    if (aPluginConf.empty()) {
      aType = Config_ModuleReader::Intrenal;
    }
  } else if (!aPluginScript.empty()) {
    aPluginName = aPluginScript;
    aType = Config_ModuleReader::Python;
  }
  if(!aPluginName.empty()) {
    myPluginTypes[aPluginName] = aType;
  }
  addDependencyModule(aPluginName);
  return aPluginName;
}

void Config_ModuleReader::loadPlugin(const std::string& thePluginName)
{
  PluginType aType = Config_ModuleReader::Binary;
  if(myPluginTypes.find(thePluginName) != myPluginTypes.end()) {
    aType = myPluginTypes.at(thePluginName);
  }
  switch (aType) {
    case Config_ModuleReader::Python:
      loadScript(thePluginName);
      break;
    case Config_ModuleReader::Binary:
    case Config_ModuleReader::Intrenal:
    default:
      loadLibrary(thePluginName);
      break;
  }
}

void Config_ModuleReader::loadScript(const std::string& theFileName)
{
  /* aquire python thread */
  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject* module = PyImport_ImportModule(theFileName.c_str());
  if (!module) {
    std::string anErrorMsg = "An error occured while importing " + theFileName;
    //Get detailed error message:
    if (PyErr_Occurred()) {
      PyObject *pstr, *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
      pstr = PyObject_Str(pvalue);
      std::string aPyError = std::string(PyString_AsString(pstr));
      if (!aPyError.empty()) {
        anErrorMsg += ":\n" + aPyError;
      }
      Py_XDECREF(pstr);
      Py_XDECREF(ptype);
      Py_XDECREF(pvalue);
      Py_XDECREF(ptraceback);
    }
    Events_Error::send(anErrorMsg);
  }

  /* release python thread */
  PyGILState_Release(gstate);
}

void Config_ModuleReader::loadLibrary(const std::string& theLibName)
{
  std::string aFileName = library(theLibName);
  if (aFileName.empty())
    return;

  #ifdef WIN32
  HINSTANCE aModLib = ::LoadLibrary(aFileName.c_str());
  #else
  void* aModLib = dlopen( aFileName.c_str(), RTLD_LAZY | RTLD_GLOBAL );
  #endif
  if(!aModLib && theLibName != "DFBrowser") { // don't show error for internal debugging tool
    std::string anErrorMsg = "Failed to load " + aFileName;
    #ifdef WIN32
    DWORD   dwLastError = ::GetLastError();
    LPSTR messageBuffer = NULL;
    size_t size = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_FROM_SYSTEM | 
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, 
                                 dwLastError, 
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                                 (LPSTR)&messageBuffer, 0, NULL);
    anErrorMsg += ": " +  std::string(messageBuffer, size);
    #else
    anErrorMsg += ": " + std::string(dlerror());
    #endif
    std::cerr << anErrorMsg << std::endl;
    Events_Error::send(anErrorMsg);
  }
}

void Config_ModuleReader::addDependencyModule(const std::string& theModuleName)
{
  myDependencyModules.insert(normalize(theModuleName));
}


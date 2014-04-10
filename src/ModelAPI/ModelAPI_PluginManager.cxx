// File:        ModelAPI_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_PluginManager.h>
// to avoid unresolved ModelAPI_Document()
#include <ModelAPI_Document.h>
// to avoid unresolved ModelAPI_Feature()
#include <ModelAPI_Feature.h>
// to avoid unresolved ModelAPI_Data()
#include <ModelAPI_Data.h>
// to avoid unresolved ModelAPI_Plugin()
#include <ModelAPI_Plugin.h>
// to avoid unresolved ModelAPI_Iterator()
#include <ModelAPI_Iterator.h>
// to avoid unresolved ModelAPI_Iterator()
#include <ModelAPI_Iterator.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

/// Converts library name to the operation system file name
string library(const string& theLibName);

/// Manager that will be initialized from Model package, one per application
std::shared_ptr<ModelAPI_PluginManager> MY_MANAGER;

ModelAPI_PluginManager::ModelAPI_PluginManager()
{
}

void ModelAPI_PluginManager::SetPluginManager(
  std::shared_ptr<ModelAPI_PluginManager> theManager)
{
  MY_MANAGER = theManager;
}

std::shared_ptr<ModelAPI_PluginManager> ModelAPI_PluginManager::get()
{
  if (!MY_MANAGER) { // import Model library that implements this interface of ModelAPI
    loadLibrary("Model");
  }
  return MY_MANAGER;
}

string library(const string& theLibName)
{
  string aLibName = theLibName;

#ifndef WIN32
  static string aLibExt( ".so" );
  if (aLibName.size() < 3 || aLibName.substr(0, 3) !="lib")
    aLibName = ".lib" + aLibName;
#else
  static string aLibExt( ".dll" );
#endif

  string anExt = aLibName.substr(aLibName.size() - 4);

  if ( anExt != aLibExt)
    aLibName += aLibExt;

  return aLibName;
}

// for debug purpose only (cerr), before the error management system is implemented
#include <iostream>
void ModelAPI_PluginManager::loadLibrary(const string theLibName)
{
  string aFileName = library(theLibName);
  if ( aFileName.empty() )
  {
    cerr<<"Library "<<theLibName.c_str()<<" can not be imported"<<endl;
    return;
  }

#ifdef WIN32
  HINSTANCE aModLib = ::LoadLibrary( aFileName.c_str() ); 
  if (!aModLib)
    cerr<<"Failed to load "<<aFileName.c_str()<<endl;
#else
  void* aModLib = dlopen( aFileName.c_str(), RTLD_LAZY );
  if ( !aModLib )
    cerr<<"Failed to load "<<aFileName.c_str()<<endl;
#endif
}

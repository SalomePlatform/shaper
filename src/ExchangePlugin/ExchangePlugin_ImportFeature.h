/*
 * ExchangePlugin_ImportFeature.h
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#ifndef EXCHANGEPLUGIN_IMPORTFEATURE_H_
#define EXCHANGEPLUGIN_IMPORTFEATURE_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>

#include <map>

#ifdef WIN32
#include <windows.h>
#define LibHandle HMODULE
#define LoadLib( name ) LoadLibrary( name )
#define GetProc GetProcAddress
#define UnLoadLib( handle ) FreeLibrary( handle );
#else
#include <dlfcn.h>
#define LibHandle void*
#define LoadLib( name ) dlopen( name, RTLD_LAZY | RTLD_GLOBAL)
#define GetProc dlsym
#define UnLoadLib( handle ) dlclose( handle );
#endif

class ExchangePlugin_ImportFeature : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("Import");
    return MY_IMPORT_ID;
  }
  /// attribute name of referenced face
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("import_file_selector");
    return MY_FILE_PATH_ID;
  }

  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportFeature();
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportFeature();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind();

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  EXCHANGEPLUGIN_EXPORT virtual bool isInHistory()
  {
    return true;
  }

 protected:
  EXCHANGEPLUGIN_EXPORT bool importFile(const std::string& theFileName);
  EXCHANGEPLUGIN_EXPORT LibHandle loadImportPlugin(const std::string& theFormatName);
  
};

#endif /* IMPORT_IMPORTFEATURE_H_ */

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_XMLModuleReader.h
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#ifndef CONFIG_MODULEREADER_H_
#define CONFIG_MODULEREADER_H_

#include <Config_def.h>
#include <Config_XMLReader.h>

#include <map>
#include <list>
#include <set>
#include <string>

/*!
 * \class Config_ModuleReader
 * \ingroup Config
 * \brief Class to process plugins.xml - definition of plugins (scripts, libraries).
 */
class Config_ModuleReader : public Config_XMLReader
{
  enum PluginType {
    Binary = 0,
    Intrenal = 1,
    Python = 2
  };

 public:
  /// Constructor
  CONFIG_EXPORT Config_ModuleReader(const char* theEventGenerated = 0);
  /// Destructor
  CONFIG_EXPORT virtual ~Config_ModuleReader();
  /// Returns map that describes which file contains a feature (the feature is key, the file is value)
  CONFIG_EXPORT const std::map<std::string, std::string>& featuresInFiles() const;
  /// Returns list of module's xml files
  CONFIG_EXPORT const std::set<std::string>& modulePluginFiles() const;
  /// Returns module name: an xml attribute from the root of the plugins.xml:
  /// e.g \code <plugins module="PartSet"> \endcode 
  CONFIG_EXPORT std::string getModuleName();
  /// Detects type of the given plugin and loads it using loadLibrary or loadScript.
  CONFIG_EXPORT static void loadPlugin(const std::string& thePluginName);
  /// loads the library with specific name, appends "lib*.dll" or "*.so" depending on the platform
  CONFIG_EXPORT static void loadLibrary(const std::string& theLibName);
  /// loads the python module with specified name
  CONFIG_EXPORT static void loadScript(const std::string& theFileName);
  /*!
   * Extends set of modules,  used for dependency checking (if there is no
   * required module in the set, a plugin will not be loaded)
   */
  CONFIG_EXPORT static void addDependencyModule(const std::string& theModuleName);

 protected:
  /// Recursively process the given xmlNode
  virtual void processNode(xmlNodePtr aNode);
  /// Defines if the reader should process children of the given node
  virtual bool processChildren(xmlNodePtr aNode);
  /// check if dependencies of the given node are in the list of loaded modules
  bool hasRequiredModules(xmlNodePtr aNode) const;
  /// reads info about plugin's features from plugin xml description
  std::list<std::string> importPlugin(const std::string& thePluginLibrary,
                                      const std::string& thePluginFile);
  /// stores information about plugin in the internal cache
  std::string addPlugin(const std::string& aPluginLibrary,
                        const std::string& aPluginScript,
                        const std::string& aPluginConf);

 private:
  std::map<std::string, std::string> myFeaturesInFiles; ///< a feature name is key, a file is value
  std::set<std::string> myPluginFiles; ///< a feature name is key, a file is value
  static std::map<std::string, PluginType> myPluginTypes; ///< a plugin name is key, a plugin type is value
  static std::set<std::string> myDependencyModules; ///< set of loaded modules
  const char* myEventGenerated; ///< gives ability to send Feature_Messages to various listeners
};

#endif /* CONFIG_XMLMODULEREADER_H_ */

// Copyright (C) 2014-2024  CEA, EDF
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  /// Returns map that describes which file contains a feature
  /// (the feature is key, the file is value)
  CONFIG_EXPORT const std::map<std::string, std::string>& featuresInFiles() const;
  /// Returns map containing features, which have licensed.
  /// The valid license should be confirmed first
  /// (the feature is key, the file is value)
  CONFIG_EXPORT const std::map<std::string, std::string>& proprietaryFeatures() const;
  /// Returns proprietary plugins
  CONFIG_EXPORT const std::set<std::string>& proprietaryPlugins() const;
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
  /// \param theFileName name of the script
  /// \param theSendErr send error message in case of faile
  CONFIG_EXPORT static void loadScript(const std::string& theFileName, bool theSendErr = true);
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
                                      const std::string& thePluginFile,
                                      const std::string& thePluginDocSection);
  /// stores information about plugin in the internal cache
  std::string addPlugin(const std::string& aPluginLibrary,
                        const std::string& aPluginScript,
                        const std::string& aPluginConf);
  /// Save feature in myFeaturesInFiles.
  /// Generates an error if the feature name is already registered.
  void addFeature(const std::string& theFeatureName, const std::string& thePluginConfig);
  /// Save feature in myFeaturesRequireLicense.
  /// Generates an error if the feature name is already registered.
  void addFeatureRequireLicense(const std::string& theFeatureName,
                                const std::string& thePluginConfig);

 private:
  std::map<std::string, std::string> myFeaturesInFiles; ///< a feature name is key, a file is value
  /// list of features, which need a license, and their config files
  std::map<std::string, std::string> myProprietaryFeatures;
  std::set<std::string> myPluginFiles; ///< a feature name is key, a file is value
  /// a plugin name is key, a plugin type is value
  static std::map<std::string, PluginType> myPluginTypes;
  static std::set<std::string> myDependencyModules; ///< set of loaded modules
  const char* myEventGenerated; ///< gives ability to send Feature_Messages to various listeners

  std::set<std::string> myProprietaryPlugins; ///< list of plugins protected by license
};

#endif /* CONFIG_XMLMODULEREADER_H_ */

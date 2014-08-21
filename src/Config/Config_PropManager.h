// File:        Config_PropManager.h
// Created:     13 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Config_PropManager_H
#define Config_PropManager_H

#include "Config_def.h"
#include "Config_Prop.h"

#include <map>
#include <string>
#include <vector>

//! Class wihich let to register properties
class CONFIG_EXPORT Config_PropManager
{
 public:

  /** 
   * Registers property parameters
   * \param theOwnerId - name of owner (name of plugin for example)
   * \param theSection - name of section (domain of using) of the property.
   * \param theName - name (title) of the value.
   * \param theType - type of the value.
   * \param theValue - initial value of the property
   * Returns True if the property succesfully registered
   */
  static bool registerProp(const std::string& theSection, const std::string& theName,
                           const std::string& theTitle, Config_Prop::PropType theType,
                           const std::string& theValue);

  static Config_Prop* findProp(const std::string& theSection, const std::string& theName);

  static Config_Properties getProperties();

  //! Returns list of registered section names.
  static std::list<std::string> getSections();

  //! Returns list of properties by its owner and section.
  static Config_Properties getProperties(const std::string& theSection);

  //! Returns value of the property by its owner, section, and name
  static std::string string(const std::string& theSection, const std::string& theName,
                            const std::string& theDefault);
  static std::vector<int> color(const std::string& theSection, const std::string& theName,
                                const std::string& theDefault);
  static int integer(const std::string& theSection, const std::string& theName,
                     const std::string& theDefault);
  static double real(const std::string& theSection, const std::string& theName,
                     const std::string& theDefault);

 private:
  static Config_Properties myProps;
};

#endif

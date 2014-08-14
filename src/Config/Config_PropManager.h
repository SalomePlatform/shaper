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
  static bool registerProp(const std::string& theOwnerId, 
                           const std::string& theSection,
                           const std::string& theName,
                           const std::string& theTitle,
                           Config_Prop::PropType theType,
                           const std::string& theValue);

  //! Returns list of registered owners.
  static std::list<std::string> getOwners();

  //! Returns list of properties by its owner.
  static Config_Properties getProperties(const std::string& theOwnerId);

  //! Returns list of registered section names.
  static std::list<std::string> getSections(const std::string& theOwnerId);

  //! Returns list of properties by its owner and section.
  static Config_Properties getProperties(const std::string& theOwnerId, const std::string& theSection);

  //! Returns value of the property by its owner, section, and name
  static std::string value(const std::string& theOwnerId, 
                           const std::string& theSection, 
                           const std::string& theName,
                           const std::string& theDefault);              

private:

  /** 
  * Registers property parameters
  * \param theOwnerId - name of owner (name of plugin for example)
  * \param theProp - the Property object.
  * Returns True if the property succesfully registered
  */
  static bool registerProp(const std::string& theOwnerId, 
                           Config_Prop* theProp);

  static std::map<std::string, Config_Properties> myPropMap;
};


CONFIG_EXPORT std::vector<int> stringToRGB(const std::string& theColor);

#endif
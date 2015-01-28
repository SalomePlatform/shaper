// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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

//! Class which let to register properties
class Config_PropManager
{
 public:

  /** 
   * Registers property parameters
   * \param theSection - name of section (domain of using) of the property.
   * \param theName - name (title) of the value.
   * \param theType - type of the value.
   * \param theDefValue - default and initial value of the property
   * Returns True if the property succesfully registered
   */
  CONFIG_EXPORT static Config_Prop* registerProp(const std::string& theSection, const std::string& theName,
    const std::string& theTitle, Config_Prop::PropType theType, const std::string& theDefValue = "");
  //! Finds property in the given section by the given name, if property not found returns NULL
  CONFIG_EXPORT static Config_Prop* findProp(
    const std::string& theSection, const std::string& theName);
  //! Returns std::list of all existing properies
  CONFIG_EXPORT static Config_Properties getProperties();
  //! Returns list of registered section names.
  CONFIG_EXPORT static std::list<std::string> getSections();
  //! Returns list of properties by its owner and section.
  CONFIG_EXPORT static Config_Properties getProperties(const std::string& theSection);

  //! Returns value of the property by its owner, section, and name
  CONFIG_EXPORT static std::string string(const std::string& theSection,
                                          const std::string& theName,
                                          const std::string& theDefault);
  //! Returns color by given section and name as 3-element vector {r,g,b}.
  CONFIG_EXPORT static std::vector<int> color(const std::string& theSection,
                                              const std::string& theName,
                                              const std::string& theDefault);
  //! Returns integer by given section and name
  CONFIG_EXPORT static int integer(const std::string& theSection,
                                   const std::string& theName,
                                   const std::string& theDefault);
  //! Returns real by given section and name
  CONFIG_EXPORT static double real(const std::string& theSection,
                                   const std::string& theName,
                                   const std::string& theDefault);

 private:
  CONFIG_EXPORT static Config_Properties myProps; ///< List of all stored properties
};

#endif

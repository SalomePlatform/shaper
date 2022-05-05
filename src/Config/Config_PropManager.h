// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef Config_PropManager_H
#define Config_PropManager_H

#include "Config_def.h"
#include "Config_Prop.h"

#include <map>
#include <string>
#include <vector>

/**
 * \class Config_PropManager
 * \ingroup Config
 * \brief Class which let to register properties
 */
class Config_PropManager
{
 public:

  /**
   * Registers property parameters
   * \param theSection - name of section (domain of using) of the property.
   * \param theName - name (title) of the value.
   * \param theTitle - title of the value.
   * \param theType - type of the value.
   * \param theDefValue - default and initial value of the property
   * \param theMin - minimal value
   * \param theMax - maximal value
   * Returns True if the property succesfully registered
   */
  CONFIG_EXPORT static Config_Prop* registerProp(const std::string& theSection,
    const std::string& theName,
    const std::string& theTitle, Config_Prop::PropType theType,
    const std::string& theDefValue = "",
    const std::string& theMin = "",
    const std::string& theMax = "");

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
                                          const std::string& theName);
  //! Returns color by given section and name as 3-element vector {r,g,b}.
  CONFIG_EXPORT static std::vector<int> color(const std::string& theSection,
                                              const std::string& theName);
  //! Returns integer by given section and name
  CONFIG_EXPORT static int integer(const std::string& theSection,
                                   const std::string& theName);
  //! Returns real by given section and name
  CONFIG_EXPORT static double real(const std::string& theSection,
                                   const std::string& theName);
  //! Returns boolean by given section and name
  CONFIG_EXPORT static bool boolean(const std::string& theSection,
                                    const std::string& theName);

  //! Returns convertion of the string to double value. Temporary changes locale to process
  //! values contained "," or "." separator.
  //! \param theDouble a value to be converted
  //! \return double result or zero
  CONFIG_EXPORT static double stringToDouble(const std::string& theDouble);

  //! Returns Auto-color status as a Boolean
  CONFIG_EXPORT static bool getAutoColorStatus();

  //! Set Auto-color boolean value
  //! \param theValue a value to use
  CONFIG_EXPORT static void setAutoColorStatus(const bool theValue);

 private:

  CONFIG_EXPORT static bool autoColorStatus; // Auto-color status

  CONFIG_EXPORT static Config_Properties& props(); ///< List of all stored properties
};

#endif

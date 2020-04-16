// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef CONFIG_PROP_H
#define CONFIG_PROP_H

#include "Config_def.h"

#include <string>
#include <list>

static const char SKETCH_TAB_NAME[] = "Sketch";

/*!
 * \class Config_Prop
 * \brief Class which describes a one property
 * \ingroup Config
 */
class Config_Prop
{
 public:

  /// Type of stored property
  enum PropType
  {
    Disabled,
    Space,
    Boolean,
    Color,
    String,
    Selector,
    DblSpin,
    IntSpin,
    Double,
    Integer,
    GroupBox,
    Tab,
    Frame,
    Font,
    DirList,
    File,
    Slider,
    Shortcut,
    ShortcutTree,
    BiColor,
    Background,
    Directory,
    Cursor
  };

  enum CursorType
  {
    ArrowCursor,
    CrossCursor,
    HandCursor
  };

  /** 
   * Creates a one property
   * \param theSection - name of section (domain of using) of the property.
   * \param theName - name (title) of the value.
   * \param theTitle - title of the value
   * \param theType - type of the value.
   * \param theDefaultValue - default value of the property. This is an initial property value
   */
  Config_Prop(const std::string& theSection, const std::string& theName,
              const std::string& theTitle, PropType theType,
              const std::string& theDefaultValue,
              const std::string& theMin, const std::string& theMax)
  {
    mySection = theSection;
    myName = theName;
    myTitle = theTitle;
    myType = theType;
    myValue = theDefaultValue;
    myDefaultValue = theDefaultValue;
    myMin = theMin;
    myMax = theMax;
  }

  /// Get name of section
  std::string section() const
  {
    return mySection;
  }
  /// Get name of property
  std::string name() const
  {
    return myName;
  }
  /// Get title of property
  std::string title() const
  {
    return myTitle;
  }
  /// Set title of property
  void setTitle(const std::string& theTitle)
  {
    myTitle = theTitle;
  }
  /// Get type of property
  PropType type() const
  {
    return myType;
  }
  /// Set type of property
  void setType(PropType theType)
  {
    myType = theType;
  }
  /// Get property's value in string format
  std::string value() const
  {
    return myValue;
  }
  /// Set property's value in string format
  CONFIG_EXPORT void setValue(const std::string& theValue);
  /// Get default value of property
  std::string defaultValue() const
  {
    return myDefaultValue;
  }
  /// Set default value of property
  CONFIG_EXPORT void setDefaultValue(const std::string& theValue);
  /// Alows to compare Config_Prop with each other
  bool operator==(const Config_Prop* theProp) const
  {
    return (mySection == theProp->section()) && (myName == theProp->name());
  }

  /// Returns minimal value
  std::string min() const { return myMin; }

  void setMin(const std::string& theMin) {
    myMin = theMin;
  }

  /// Returns maximal value
  std::string max() const { return myMax; }

  void setMax(const std::string& theMax) {
    myMax = theMax;
  }

 private:
  std::string mySection; ///< Name of section
  std::string myName; ///< Name of property
  std::string myTitle; ///< Title of property
  PropType myType; ///< Type of property
  std::string myValue; // Value in string format
  std::string myDefaultValue; // Default value
  std::string myMin; // Minimal value
  std::string myMax; // Maximal value
};

typedef std::list<Config_Prop*> Config_Properties;

#endif

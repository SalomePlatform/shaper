// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_Prop.h
// Created:     12 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Config_Prop_H
#define Config_Prop_H

#include "Config_def.h"

#include <string>
#include <list>

/// Class which describes a one property
class Config_Prop
{
 public:

  /// Type of stored property
  enum PropType
  {
    Disabled,
    Space,
    Bool,
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
    Directory
  };

  /** 
   * Creates a one property
   * \param theSection - name of section (domain of using) of the property.
   * \param theName - name (title) of the value.
   * \param theType - type of the value.
   * \param theDefaultValue - default value of the property. This is an initial property value
   */
  Config_Prop(const std::string& theSection, const std::string& theName,
              const std::string& theTitle, PropType theType, const std::string& theDefaultValue)
  {
    mySection = theSection;
    myName = theName;
    myTitle = theTitle;
    myType = theType;
    myValue = theDefaultValue;
    myDefaultValue = theDefaultValue;
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

 private:
  std::string mySection; ///< Name of section
  std::string myName; ///< Name of property
  std::string myTitle; ///< Title of property
  PropType myType; ///< Type of property
  std::string myValue; // Value in string format
  std::string myDefaultValue; // Default value
};

typedef std::list<Config_Prop*> Config_Properties;

#endif

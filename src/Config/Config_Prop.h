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
    Background
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

  std::string section() const
  {
    return mySection;
  }
  std::string name() const
  {
    return myName;
  }

  std::string title() const
  {
    return myTitle;
  }
  void setTitle(const std::string& theTitle)
  {
    myTitle = theTitle;
  }

  PropType type() const
  {
    return myType;
  }
  void setType(PropType theType)
  {
    myType = theType;
  }

  std::string value() const
  {
    return myValue;
  }

  CONFIG_EXPORT void setValue(const std::string& theValue);
  
  std::string defaultValue() const
  {
    return myDefaultValue;
  }

  CONFIG_EXPORT void setDefaultValue(const std::string& theValue);

  bool operator==(const Config_Prop* theProp) const
  {
    return (mySection == theProp->section()) && (myName == theProp->name());
  }

 private:
  std::string mySection;
  std::string myName;
  std::string myTitle;
  PropType myType;
  std::string myValue;
  std::string myDefaultValue;
};

typedef std::list<Config_Prop*> Config_Properties;

#endif

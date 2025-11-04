// Copyright (C) 2014-2025  CEA, EDF
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

#include "Config_PropManager.h"
#include <clocale>
bool Config_PropManager::autoColorStatus = false;

std::vector<int> stringToRGB(const std::string& theColor);
int stringToInteger(const std::string& theInt);
bool stringToBoolean(const std::string& theInt);

Config_Properties& Config_PropManager::props() {
  static Config_Properties* confProps = new Config_Properties();
  return *confProps;
}

bool Config_PropManager::getAutoColorStatus()
{
    return Config_PropManager::autoColorStatus;
}

void Config_PropManager::setAutoColorStatus(const bool theValue)
{
    Config_PropManager::autoColorStatus = theValue;
}

Config_Prop* Config_PropManager::registerProp(const std::string& theSection,
                                              const std::string& theName,
                                              const std::string& theTitle,
                                              Config_Prop::PropType theType,
                                              const std::string& theDefaultValue,
                                              const std::string& theMin,
                                              const std::string& theMax)
{
  Config_Prop* aProp = findProp(theSection, theName);

  if (aProp) {
    if (aProp->value() == "") {
      aProp->setValue(theDefaultValue);
    }
    if (aProp->defaultValue() == "") {
      aProp->setDefaultValue(theDefaultValue);
    }
    if (aProp->type() == Config_Prop::Disabled) {
      aProp->setType(theType);
      aProp->setTitle(theTitle);
    }
    if (theMin != "")
      aProp->setMin(theMin);
    if (theMax != "")
      aProp->setMax(theMax);
  }
  else {
    aProp =
      new Config_Prop(theSection, theName, theTitle, theType, theDefaultValue, theMin, theMax);
    props().push_back(aProp);
  }
  return aProp;
}

Config_Prop* Config_PropManager::findProp(const std::string& theSection, const std::string& theName)
{
  Config_Properties::const_iterator aIt;
  Config_Properties aProps = props();
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    if ((aProp->section() == theSection) && (aProp->name() == theName))
      return aProp;
  }
  return NULL;
}

Config_Properties Config_PropManager::getProperties()
{
  Config_Properties aRes;
  Config_Properties::const_iterator aIt;
  Config_Properties aProps = props();
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if (aProp->type() != Config_Prop::Disabled)
      aRes.push_back(aProp);
  }
  return aRes;
}

std::list<std::string> Config_PropManager::getSections()
{
  // Return only non disabled sections
  std::list<std::string> aSections;
  Config_Properties::const_iterator aIt;
  Config_Properties aProps = props();
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    const Config_Prop* aProp = (*aIt);
    if (aProp->type() != Config_Prop::Disabled)
      aSections.push_back(aProp->section());
  }
  aSections.unique();
  return aSections;
}

Config_Properties Config_PropManager::getProperties(const std::string& theSection)
{
  Config_Properties aRes;
  Config_Properties::iterator aIt;
  Config_Properties aProps = props();
  for (aIt = aProps.begin(); aIt != aProps.end(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if ((aProp->section() == theSection) && (aProp->type() != Config_Prop::Disabled))
      aRes.push_back(aProp);
  }
  return aRes;
}

std::string Config_PropManager::string(const std::string& theSection, const std::string& theName)
{
  Config_Properties aProps = getProperties(theSection);
  Config_Properties::const_iterator aIt;
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if (aProp->name() == theName)
      return aProp->value();
  }
  std::string aMsg = "Property " + theSection + ":" + theName + " is not registered";
  throw aMsg;
}

std::vector<int> Config_PropManager::color(const std::string& theSection,
                                           const std::string& theName)
{
  std::string aStr = string(theSection, theName);
  return stringToRGB(aStr);
}

int Config_PropManager::integer(const std::string& theSection, const std::string& theName)
{
  std::string aStr = string(theSection, theName);
  return stringToInteger(aStr);
}

double Config_PropManager::real(const std::string& theSection, const std::string& theName)
{
  std::string aStr = string(theSection, theName);
  return stringToDouble(aStr);
}

bool Config_PropManager::boolean(const std::string& theSection,
                                 const std::string& theName)
{
  std::string aStr = string(theSection, theName);
  return stringToBoolean(aStr);
}

std::vector<int> stringToRGB(const std::string& theColor)
{
  std::vector<int> aRes(3);

  if (theColor[0] == '#') {
    char aBuf[3];
    char* aP;
    aBuf[2] = '\0';

    aBuf[0] = theColor[1];
    aBuf[1] = theColor[2];
    aRes[0] = strtol(aBuf, &aP, 16);

    aBuf[0] = theColor[3];
    aBuf[1] = theColor[4];
    aRes[1] = strtol(aBuf, &aP, 16);

    aBuf[0] = theColor[5];
    aBuf[1] = theColor[6];
    aRes[2] = strtol(aBuf, &aP, 16);
  } else {
    int aPos = (int)theColor.find(",");
    char aBuf[10];
    // Get Red
    std::size_t length = theColor.copy(aBuf, aPos, 0);
    aBuf[length] = '\0';
    aRes[0] = atoi(aBuf);

    // Get Green
    int aNPos = (int)theColor.find(",", aPos + 1);
    length = theColor.copy(aBuf, aNPos - aPos - 1, aPos + 1);
    aBuf[length] = '\0';
    aRes[1] = atoi(aBuf);

    // Get Green
    length = theColor.copy(aBuf, theColor.length() - aNPos - 1, aNPos + 1);
    aBuf[length] = '\0';
    aRes[2] = atoi(aBuf);
  }
  return aRes;
}

int stringToInteger(const std::string& theInt)
{
  return atoi(theInt.c_str());
}

double Config_PropManager::stringToDouble(const std::string& theDouble)
{
  std::string aStr = theDouble;

  // change locale and convert "," to "." if exists
  std::string aCurLocale = setlocale(LC_NUMERIC, 0);
  setlocale(LC_NUMERIC, "C");
  size_t dotpos = aStr.find(',');
  if (dotpos != std::string::npos)
    aStr.replace(dotpos, 1, ".");

  char* p;
  double aValue = strtod(aStr.c_str(), &p);

  // restore locale
  setlocale(LC_NUMERIC, aCurLocale.c_str());
  return aValue;
}

bool stringToBoolean(const std::string& theBoolean)
{
  return theBoolean == "true";
}

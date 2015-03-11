// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_PropManager.cpp
// Created:     13 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "Config_PropManager.h"

std::vector<int> stringToRGB(const std::string& theColor);
int stringToInteger(const std::string& theInt);
double stringToDouble(const std::string& theDouble);
bool stringToBoolean(const std::string& theInt);

Config_Properties Config_PropManager::myProps;

Config_Prop* Config_PropManager::registerProp(const std::string& theSection, const std::string& theName,
                                              const std::string& theTitle, Config_Prop::PropType theType,
                                              const std::string& theDefaultValue)
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
  }
  else {
    aProp = new Config_Prop(theSection, theName, theTitle, theType, theDefaultValue);
    myProps.push_back(aProp);
  }
  return aProp;
}

Config_Prop* Config_PropManager::findProp(const std::string& theSection, const std::string& theName)
{
  Config_Properties::const_iterator aIt;
  for (aIt = myProps.cbegin(); aIt != myProps.cend(); ++aIt) {
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
  for (aIt = myProps.cbegin(); aIt != myProps.cend(); aIt++) {
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
  for (aIt = myProps.cbegin(); aIt != myProps.cend(); aIt++) {
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
  for (aIt = myProps.begin(); aIt != myProps.end(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if ((aProp->section() == theSection) && (aProp->type() != Config_Prop::Disabled))
      aRes.push_back(aProp);
  }
  return aRes;
}

std::string Config_PropManager::string(const std::string& theSection, const std::string& theName,
                                       const std::string& theDefault)
{
  Config_Properties aProps = getProperties(theSection);
  Config_Properties::const_iterator aIt;
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if (aProp->name() == theName)
      return aProp->value();
  }
  return theDefault;
}

std::vector<int> Config_PropManager::color(const std::string& theSection,
                                           const std::string& theName,
                                           const std::string& theDefault)
{
  std::string aStr = string(theSection, theName, theDefault);
  return stringToRGB(aStr);
}

int Config_PropManager::integer(const std::string& theSection, const std::string& theName,
                                const std::string& theDefault)
{
  std::string aStr = string(theSection, theName, theDefault);
  return stringToInteger(aStr);
}

double Config_PropManager::real(const std::string& theSection, const std::string& theName,
                                const std::string& theDefault)
{
  std::string aStr = string(theSection, theName, theDefault);
  return stringToDouble(aStr);
}

bool Config_PropManager::boolean(const std::string& theSection,
                                 const std::string& theName,
                                 const std::string& theDefault)
{
  std::string aStr = string(theSection, theName, theDefault);
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
    int aPos = theColor.find(",");
    char aBuf[10];
    // Get Red
    std::size_t length = theColor.copy(aBuf, aPos, 0);
    aBuf[length] = '\0';
    aRes[0] = atoi(aBuf);

    // Get Green
    int aNPos = theColor.find(",", aPos + 1);
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

double stringToDouble(const std::string& theDouble)
{
  char* p;
  return strtod(theDouble.c_str(), &p);
}

bool stringToBoolean(const std::string& theBoolean)
{
  return theBoolean == "true";
}

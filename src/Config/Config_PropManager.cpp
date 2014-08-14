// File:        Config_PropManager.cpp
// Created:     13 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "Config_PropManager.h"


std::map<std::string, Config_Properties> Config_PropManager::myPropMap;



bool Config_PropManager::registerProp(const std::string& theOwnerId, 
                                      const std::string& theSection,
                                      const std::string& theName,
                                      const std::string& theTitle,
                                      Config_Prop::PropType theType,
                                      const std::string& theValue)
{
  Config_Prop* aProp = new Config_Prop(theSection, theName, theTitle, theType, theValue);
  return registerProp(theOwnerId, aProp);
}

bool Config_PropManager::registerProp(const std::string& theOwnerId, 
                                      Config_Prop* theProp)
{
  if (myPropMap.find(theOwnerId) == myPropMap.end()) {
    Config_Properties aPropList;
    aPropList.push_back(theProp);
    myPropMap[theOwnerId] = aPropList;
  } else {
    myPropMap[theOwnerId].push_back(theProp);
  }
  return true;
}

std::list<std::string> Config_PropManager::getOwners()
{
  std::list<std::string> aKeys;
  std::map<std::string, Config_Properties>::const_iterator aIt;
  for (aIt = myPropMap.cbegin(); aIt != myPropMap.cend(); ++aIt) 
    aKeys.push_back((*aIt).first);
  return aKeys;
}

Config_Properties Config_PropManager::getProperties(const std::string& theOwnerId)
{
  return myPropMap[theOwnerId];
}


std::list<std::string> Config_PropManager::getSections(const std::string& theOwnerId)
{
  std::list<std::string> aSections;
  Config_Properties aProps = getProperties(theOwnerId);
  Config_Properties::const_iterator aIt;
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    const Config_Prop* aProp = (*aIt);
    aSections.push_back(aProp->section());
  }
  aSections.unique();
  return aSections;
}

Config_Properties Config_PropManager::getProperties(const std::string& theOwnerId, 
                                                    const std::string& theSection)
{
  Config_Properties aProps = getProperties(theOwnerId);
  Config_Properties aRes;
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if (aProp->section() == theSection)
      aRes.push_back(aProp);
  }
  return aRes;
}


std::string Config_PropManager::value(const std::string& theOwnerId, 
                                      const std::string& theSection, 
                                      const std::string& theName,
                                      const std::string& theDefault)
{
  Config_Properties aProps = getProperties(theOwnerId, theSection);
  Config_Properties::const_iterator aIt;
  for (aIt = aProps.cbegin(); aIt != aProps.cend(); aIt++) {
    Config_Prop* aProp = (*aIt);
    if (aProp->name() == theName)
      return aProp->value();
  }
  return theDefault;
}


std::vector<int> stringToRGB(const std::string& theColor)
{
  std::vector<int> aRes(3);

  int aPos = theColor.find(",");
  char aBuf[10];
  // Get Red
  std::size_t length = theColor.copy(aBuf, aPos, 0);
  aBuf[length] = '\0';
  int aC = atoi(aBuf);
  aRes[0] = aC;

  // Get Green
  int aNPos = theColor.find(",", aPos + 1);
  length = theColor.copy(aBuf, aNPos - aPos - 1, aPos + 1);
  aBuf[length] = '\0';
  aC = atoi(aBuf);
  aRes[1] = aC;

  // Get Green
  length = theColor.copy(aBuf, theColor.length() - aNPos - 1, aNPos + 1);
  aBuf[length] = '\0';
  aC = atoi(aBuf);
  aRes[2] = aC;

  return aRes;
}
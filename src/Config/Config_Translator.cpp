// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_Translator.cpp
// Created:     31 May 2016
// Author:      Vitaly SMETANNIKOV

#include "Config_Translator.h"
#include "Config_XMLReader.h"
#include "Config_Common.h"

class Config_TSReader : public Config_XMLReader
{
public:
  Config_TSReader(const std::string& theTSFile) : Config_XMLReader(theTSFile) {}

  const Config_Translator::Translator& translator() const { return myTranslator; }

protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr theNode);
private:
  Config_Translator::Translator myTranslator;
};

void Config_TSReader::processNode(xmlNodePtr theNode)
{
  static std::string aName;
  static std::string aSource;
  std::string aTranslat;

  if (isNode(theNode, "context", NULL)) {
    aName = "";
  } else if (isNode(theNode, "name", NULL)) {
    aName = getContent(theNode);
  } else if (isNode(theNode, "message", NULL)) {
    aSource = "";
  } else if (isNode(theNode, "source", NULL)) {
    aSource = getContent(theNode);
  } else if (isNode(theNode, "translation", NULL)) {
    aTranslat = getContent(theNode);
    if ((aName.size() > 0) && (aSource.size() > 0))
      myTranslator[aName][aSource] = aTranslat;
  }
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
Config_Translator::Translator Config_Translator::myTranslator;

bool Config_Translator::load(const std::string& theFileName)
{
  Config_TSReader aReader(theFileName);
  aReader.readAll();

  const Translator& aTranslator = aReader.translator();
  Translator::const_iterator aIt;
  std::string aContext;
  Dictionary aDictionary;
  for (aIt = aTranslator.cbegin(); aIt != aTranslator.cend(); aIt++) {
    aContext = (*aIt).first;
    aDictionary = (*aIt).second;
    if (myTranslator.count(aContext) == 0) {
      myTranslator[aContext] = aDictionary;
    } else {
      Dictionary::const_iterator aDictIt;
      for (aDictIt = aDictionary.cbegin(); aDictIt != aDictionary.cend(); aDictIt++) {
        myTranslator[aContext][(*aDictIt).first] = (*aDictIt).second;
      }
    }
  }

  return true;
}

std::string Config_Translator::translate(const Events_InfoMessage& theInfo)
{
  std::string aContext = theInfo.context();
  std::string aMessage = theInfo.message();
  std::list<std::string> aParameters = theInfo.parameters();
  if (myTranslator.count(aContext) > 0) {
    if (myTranslator[aContext].count(aMessage) > 0) {
      std::string aTranslation = myTranslator[aContext][aMessage];
      if (aParameters.size() > 0) {
        std::list<std::string>::const_iterator aIt;
        int i;
        char aBuf[20];
        std::string aParam;
        for (i=1, aIt = aParameters.cbegin(); aIt != aParameters.cend(); aIt++, i++) {
          aParam = (*aIt);
          sprintf(aBuf, "%d ", i);
          std::string aCode = std::string("%") + std::string(aBuf);
          size_t aPos = aTranslation.find(aCode);
          if (aPos != std::string::npos) {
            std::string aFirst = aTranslation.substr(0, aPos);
            std::string aLast = aTranslation.substr(aPos + aCode.length(), std::string::npos);
            aTranslation = aFirst + aParam + aLast;
          }
        }
      }
      return aTranslation;
    }
  }
  return "";
}

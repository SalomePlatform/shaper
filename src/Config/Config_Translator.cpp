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

std::string Config_Translator::translate(std::shared_ptr<Events_InfoMessage> theInfo)
{
  std::string aContext = theInfo->context();
  std::string aMessage = theInfo->message();
  std::list<std::string> aParameters = theInfo->parameters();
  return translate(aContext, aMessage, aParameters);
}


std::string insertParameters(const std::string& theString, const std::list<std::string>& theParams)
{
  std::string aResult = theString;
  std::list<std::string>::const_iterator aIt;
  int i;
  char aBuf[20];
  std::string aParam;
  for (i=1, aIt = theParams.cbegin(); aIt != theParams.cend(); aIt++, i++) {
    aParam = (*aIt);
    sprintf_s(aBuf, "%d", i);
    std::string aCode = std::string("%") + std::string(aBuf);
    size_t aPos = aResult.find(aCode);
    if (aPos != std::string::npos) {
      std::string aFirst = aResult.substr(0, aPos);
      std::string aLast = aResult.substr(aPos + aCode.length(), std::string::npos);
      aResult = aFirst + aParam + aLast;
    }
  }
  return aResult;
}

std::string Config_Translator::translate(const std::string& theContext,
                                         const std::string& theMessage, 
                                         const std::list<std::string>& theParams)
{
  if (myTranslator.count(theContext) > 0) {
    if (myTranslator[theContext].count(theMessage) > 0) {
      std::string aTranslation = myTranslator[theContext][theMessage];
      if (theParams.size() > 0) {
        aTranslation = insertParameters(aTranslation, theParams);
      }
      return aTranslation;
    }
  }
  std::string aMsg = theMessage;
  if (theParams.size() > 0) {
    aMsg = insertParameters(aMsg, theParams);
  }
  return aMsg;
}

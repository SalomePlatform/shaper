// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_Translator.cpp
// Created:     31 May 2016
// Author:      Vitaly SMETANNIKOV

#include "Config_Translator.h"
#include <Config_XMLReader.h>

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
}


bool Config_Translator::load(const std::string& theFileName)
{
  Config_TSReader aReader(theFileName);
  aReader.readAll();

  const Translator& aTranslator = aReader.translator();
  Translator::const_iterator aIt;
  for (aIt = aTranslator.cbegin(); aIt != aTranslator.cend(); aIt++) {
  }

  return true;
}

std::string Config_Translator::translate(const Events_InfoMessage& theInfo)
{
  return "";
}

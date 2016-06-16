// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_Translator.cpp
// Created:     31 May 2016
// Author:      Vitaly SMETANNIKOV

#include "Config_Translator.h"
#include "Config_XMLReader.h"
#include "Config_Common.h"

#include <fstream>
#include <ostream>

/**
 * \class Config_TSReader
 * \ingroup Config
 * \brief Class for reading translations from TS files (an XML format).
 */
class Config_TSReader : public Config_XMLReader
{
public:
  /// Constructor
  /// \param theTSFile name of TS file
  Config_TSReader(const std::string& theTSFile) : Config_XMLReader(theTSFile) {}

  /// Returns content of TS file
  const Config_Translator::Translator& translator() const { return myTranslator; }

  /// Returns codecs defined in TS files
  const Config_Translator::Dictionary& codecs() const { return myCodecs; }

protected:
  /// Overloaded method. Defines how to process each node
  virtual void processNode(xmlNodePtr theNode);
private:
  Config_Translator::Translator myTranslator;
  Config_Translator::Dictionary myCodecs;
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
    myCodecs[aName] = encoding();
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
Config_Translator::Dictionary Config_Translator::myCodecs;

#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
Config_Translator::Translator Config_Translator::myMissed;
#endif
#endif

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

  const Dictionary aCodecs = aReader.codecs();
  Dictionary::const_iterator aDictIt;
  for (aDictIt = aCodecs.cbegin(); aDictIt != aCodecs.cend(); aDictIt++) {
    myCodecs[aDictIt->first] = aDictIt->second;
  }
  return true;
}

std::string Config_Translator::translate(std::shared_ptr<Events_InfoMessage> theInfo)
{
  std::string aContext = theInfo->context();
  std::string aMessage = theInfo->messageString();
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
    sprintf(aBuf, "%d", i);
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
      if (aTranslation.size() > 0)
        return aTranslation;
    }
  }
  std::string aMsg = theMessage;
  if (theParams.size() > 0) {
    aMsg = insertParameters(aMsg, theParams);
  }
#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
  myMissed[theContext][theMessage] = "";
#endif
#endif
  return aMsg;
}


std::string Config_Translator::codec(const std::string& theContext)
{ 
  return (myCodecs.count(theContext) > 0)? myCodecs[theContext] : "UTF-8"; 
}


#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
void Config_Translator::saveMissedTranslations()
{
  if (myMissed.size() == 0)
    return;

  char* aPath = getenv("ROOT_DIR");
#ifdef WIN32
  std::string aFile = aPath + std::string("\\MissedTranslation.ts");
#else
  std::string aFile = aPath + std::string("/MissedTranslation.ts");
#endif
  std::ofstream oFStream;

  // Delete old file
  int aa = remove(aFile.c_str());

  oFStream.open(aFile, std::ofstream::out | std::ofstream::app);
  if (oFStream.is_open()) {
    Translator::const_iterator aContIt;
    Dictionary::const_iterator aDictIt;
    std::string aContext;
    std::string aSrc;
    Dictionary aDict;

    oFStream << "<TS version=\"2.0\">" << std::endl;
    for(aContIt = myMissed.cbegin(); aContIt != myMissed.cend(); aContIt++) {
      oFStream << "<context>" << std::endl;

      aContext = aContIt->first;
      oFStream << "  <name>" << aContext << "</name>" << std::endl;

      aDict = aContIt->second;
      for(aDictIt = aDict.cbegin(); aDictIt != aDict.cend(); aDictIt++) {
        oFStream << "  <message>" << std::endl;
        aSrc = aDictIt->first;

        oFStream << "    <source>" << aSrc << "</source>" << std::endl;
        oFStream << "    <translation>" << "</translation>" << std::endl;

        oFStream << "  </message>" << std::endl;
      }
      oFStream << "</context>" << std::endl;
    }

    oFStream << "</TS>" << std::endl;
    oFStream.close();
  }
}
#endif
#endif

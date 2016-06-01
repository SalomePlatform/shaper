// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Config_Translator.h
// Created:     31 May 2016
// Author:      Vitaly SMETANNIKOV

#ifndef Config_Translator_H
#define Config_Translator_H

#include "Config_def.h"
#include <Events_InfoMessage.h>

#include <string>
#include <map>

class Config_Translator
{
public:
  /// A data type of dictionary <KeyString, ResultString>
  typedef std::map<std::string, std::string> Dictionary;

  /// A data type of Translator with structure <Context, Dictionary>
  typedef std::map<std::string, Dictionary> Translator;

  /**
  * Load translations from TS file
  * \param theFileName a TS file name with full path
  */
  static CONFIG_EXPORT bool load(const std::string& theFileName);

  /**
  * Returns translation from the given info message.
  * If transdlation is not exists then it returns a string 
  * from the info data without translation
  * \param theInfo an info message
  */
  static CONFIG_EXPORT std::string translate(const Events_InfoMessage& theInfo);

private:
  static Translator myTranslator;
};

#endif
// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Config_Translator_H
#define Config_Translator_H

#include "Config_def.h"
#include <Events_InfoMessage.h>

#include <string>
#include <map>


/// The definition provides collection and writing of missed translations
//#define MISSED_TRANSLATION

/**
 * \class Config_Translator
 * \ingroup Config
 * \brief Class for messages translation on different languages. It can load TS
 * files wich contain translation string and provides translations of messages from source code
 */
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
  * If translation is not exists then it returns a string
  * from the info data without translation
  * \param theInfo an info message
  */
  static CONFIG_EXPORT std::string translate(const Events_InfoMessage& theInfo);

  /**
  * Returns translation from the given data.
  * If translation is not exists then it returns a string 
  * from the info data without translation
  * \param theContext context of the message (Feature Id)
  * \param theMessage a message which dave to be translated
  * \param theParams a list of parameters (can be empty)
  */
  static CONFIG_EXPORT std::string translate(const std::string& theContext,
    const std::string& theMessage,
    const std::list<std::string>& theParams = std::list<std::string>());


  /**
  * Returns codec for the context
  * \param theContext the context
  */ 
  static CONFIG_EXPORT std::string codec(const std::string& theContext);

  /**
  * Returns codec for the context
  * \param theInfo the info
  */ 
  static CONFIG_EXPORT std::string codec(const Events_InfoMessage& theInfo);

#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
  static CONFIG_EXPORT void saveMissedTranslations();
#endif
#endif


private:
  /// A map of translations
  static Translator myTranslator;

  /// aMap of codecs for contexts
  static Dictionary myCodecs;

#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
  static Translator myMissed;
#endif
#endif
};

#endif

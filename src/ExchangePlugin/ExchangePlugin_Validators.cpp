// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Validators.cpp
// Created: Aug 01, 2014
// Author:  Sergey BELASH

#include <ExchangePlugin_Validators.h>

#include <ExchangePlugin_Tools.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>

#include <list>
#include <string>
#include <algorithm>

bool ExchangePlugin_FormatValidator::parseFormats(const std::list<std::string>& theArguments,
                                                  std::list<std::string>& outFormats)
{
  std::list<std::string>::const_iterator it = theArguments.begin();
  bool result = true;
  for (; it != theArguments.end(); ++it) {
    std::string anArg = *it;
    int aSepPos = anArg.find(":");
    if (aSepPos == std::string::npos) {
      result = false;
      continue;
    }
    std::string aFormats = anArg.substr(0, aSepPos);
    std::transform(aFormats.begin(), aFormats.end(), aFormats.begin(), toupper);
    std::list<std::string> aFormatList = ExchangePlugin_Tools::split(aFormats, '|');
    outFormats.insert(outFormats.end(), aFormatList.begin(), aFormatList.end());
  }
  return result;
}

bool ExchangePlugin_FormatValidator::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const
{
  if (!theAttribute->isInitialized()) {
    theError = "Is not initialized.";
    return false;
  }

  const AttributeStringPtr aStrAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr) {
    theError = "Is not a string attribute.";
    return false;
  }

  std::string aFileName = aStrAttr->value();
  if (aFileName.empty()) {
    theError = "File name is empty.";
    return false;
  }

  std::list<std::string> aFormats;
  ExchangePlugin_FormatValidator::parseFormats(theArguments, aFormats);
  std::list<std::string>::const_iterator itFormats = aFormats.begin();
  size_t aFileNameLen = aFileName.length();
  std::transform(aFileName.begin(), aFileName.end(), aFileName.begin(), toupper);
  // Is file name ends with the format
  for (; itFormats != aFormats.end(); ++itFormats) {
    if (aFileNameLen > (*itFormats).length()) {
      size_t aFormatBeginPos = aFileNameLen - (*itFormats).length();
      if (aFileName.compare(aFormatBeginPos, std::string::npos, *itFormats) == 0) {
        return true;
      }
    }
  }
  theError = "File name does not end with any available format.";
  return false;
}

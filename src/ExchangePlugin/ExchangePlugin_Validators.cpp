// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include <ExchangePlugin_Validators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>

#include <list>
#include <string>
#include <algorithm>

bool ExchangePlugin_ImportFormatValidator::parseFormats(const std::list<std::string>& theArguments,
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
    std::string aFormat = anArg.substr(0, aSepPos);
    std::transform(aFormat.begin(), aFormat.end(), aFormat.begin(), toupper);
    outFormats.push_back(aFormat);
  }
  return result;
}

bool ExchangePlugin_ImportFormatValidator::parsePlugins(const std::list<std::string>& theArguments,
                                                        std::list<std::string>& outPlugins)
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
    outPlugins.push_back(anArg.substr(aSepPos + 1));
  }
  return result;
}

bool ExchangePlugin_ImportFormatValidator::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments) const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  if (theAttribute->isInitialized()) {
    const AttributeStringPtr aStrAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
    if(!aStrAttr)
      return false;
    std::string aFileName = aStrAttr->value();
    if (!aFileName.empty()) {
      std::list<std::string> aFormats;
      ExchangePlugin_ImportFormatValidator::parseFormats(theArguments, aFormats);
      std::list<std::string>::const_iterator itFormats = aFormats.begin();
      size_t aFileNameLen = aFileName.length();
      std::transform(aFileName.begin(), aFileName.end(), aFileName.begin(), toupper);
      // Is file name ends with the format
      for (; itFormats != aFormats.end(); ++itFormats) {
        size_t aFormatBeginPos = aFileNameLen - (*itFormats).length();
        if (aFileName.compare(aFormatBeginPos, std::string::npos, *itFormats) == 0) {
          return true;
        }
      }
    }
  }
  return false;
}


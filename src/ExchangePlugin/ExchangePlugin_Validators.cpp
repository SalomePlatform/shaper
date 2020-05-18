// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ExchangePlugin_Validators.h>

#include <ExchangePlugin_Tools.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>

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
    size_t aSepPos = anArg.find(":");
    if (aSepPos == std::string::npos) {
      result = false;
      continue;
    }
    std::string aFormats = anArg.substr(0, aSepPos);
    std::transform(aFormats.begin(), aFormats.end(), aFormats.begin(),
                   [](char c) { return static_cast<char>(::toupper(c)); });
    std::list<std::string> aFormatList = ExchangePlugin_Tools::split(aFormats, '|');
    outFormats.insert(outFormats.end(), aFormatList.begin(), aFormatList.end());
  }
  return result;
}

bool ExchangePlugin_FormatValidator::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const
{
  if (!theAttribute->isInitialized()) {
// LCOV_EXCL_START
    theError = "%1 is not initialized.";
    theError.arg(theAttribute->id());
    return false;
// LCOV_EXCL_STOP
  }

  const AttributeStringPtr aStrAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr) {
// LCOV_EXCL_START
    theError = "%1 is not a string attribute.";
    theError.arg(theAttribute->id());
    return false;
// LCOV_EXCL_STOP
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
  std::transform(aFileName.begin(), aFileName.end(), aFileName.begin(),
                 [](char c) { return static_cast<char>(::toupper(c)); });
  // Is file name ends with the format
  for (; itFormats != aFormats.end(); ++itFormats) {
    std::string aFormat = "." + *itFormats;
    if (aFileNameLen > aFormat.length()) {
      size_t aFormatBeginPos = aFileNameLen - aFormat.length();
      if (aFileName.compare(aFormatBeginPos, std::string::npos, aFormat) == 0) {
        return true;
      }
    }
  }
  theError = "File name does not end with any available format.";
  return false;
}


bool ExchangePlugin_InHistoryValidator::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttrSelection =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Context is empty.";
      return false;
    }

    FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
    if (!aFeature->isInHistory()) {
      theError = "Error: Feature is not in history.";
      return false;
    }
  } else if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

    // All objects should not be result constructions.
    for(int anIndex = 0, aSize = anAttrSelectionList->size(); anIndex < aSize; ++anIndex) {
      AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
      if(!isValid(anAttrSelection, theArguments, theError)) {
        return false;
      }
    }
  } else {
// LCOV_EXCL_START
    theError = "Error: Attribute \"%1\" does not supported by this validator.";
    theError.arg(anAttributeType);
    return false;
// LCOV_EXCL_STOP
  }

  return true;
}

// Copyright (C) 2014-2024  CEA, EDF
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

#include "ExchangePlugin_Import.h"
#include "ExchangePlugin_ImportFeature.h"

#include <Locale_Convert.h>

#include <PartSetPlugin_Part.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>


static const std::wstring THE_NEW_PART_STR(L"New Part");

DocumentPtr findDocument(DocumentPtr thePartSetDoc, const std::wstring& thePartName)
{
  DocumentPtr aDoc;
  FeaturePtr aPartFeature;
  if (thePartName == THE_NEW_PART_STR) {
    // create new part
    aPartFeature = thePartSetDoc->addFeature(PartSetPlugin_Part::ID());
    if (aPartFeature)
      aPartFeature->execute();
  }
  else {
    // find existing part by its name
    std::list<FeaturePtr> aSubFeatures = thePartSetDoc->allFeatures();
    for (std::list<FeaturePtr>::iterator aFIt = aSubFeatures.begin();
      aFIt != aSubFeatures.end(); ++aFIt) {
      if ((*aFIt)->getKind() == PartSetPlugin_Part::ID() && (*aFIt)->name() == thePartName) {
        aPartFeature = *aFIt;
        break;
      }
    }
  }

  if (aPartFeature) {
    ResultPartPtr aPartResult =
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->lastResult());
    if (aPartResult)
      aDoc = aPartResult->partDoc();
  }
  return aDoc;
}


/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ImportBase::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TARGET_PART_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(TARGET_PARTS_LIST_ID(), ModelAPI_AttributeStringArray::typeId());
}

void ExchangePlugin_Import::initAttributes()
{
  ExchangePlugin_ImportBase::initAttributes();

  data()->addAttribute(STEP_FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(IMPORT_TYPE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(STEP_TARGET_PART_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(STEP_TARGET_PARTS_LIST_ID(), ModelAPI_AttributeStringArray::typeId());
  data()->addAttribute(STEP_MATERIALS_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(STEP_COLORS_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(STEP_SCALE_INTER_UNITS_ID(), ModelAPI_AttributeBoolean::typeId());

#ifndef HAVE_SALOME
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::IMPORT_TYPE_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_FILE_PATH_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_TARGET_PART_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_TARGET_PARTS_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_MATERIALS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_COLORS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), ExchangePlugin_Import::STEP_SCALE_INTER_UNITS_ID());
#endif
}

/*
 * Computes or recomputes the results
 */

void ExchangePlugin_Import::execute()
{
  AttributeStringPtr aFormatAttr =
      this->string(ExchangePlugin_Import::IMPORT_TYPE_ID());
  std::string aFormat = aFormatAttr->value();
  AttributeStringPtr aFilePathAttr;
  std::string aFilePath;
  AttributeStringArrayPtr aPartsAttr;
  AttributeIntegerPtr aTargetAttr;
  if (aFormat == "STEP" || aFormat == "STP")
  {
    aFilePathAttr = string(ExchangePlugin_Import::STEP_FILE_PATH_ID());
    aFilePath = aFilePathAttr->value();
    // get the document where to import
    aPartsAttr = stringArray(STEP_TARGET_PARTS_LIST_ID());
    aTargetAttr = integer(STEP_TARGET_PART_ID());
  }else{
    aFilePathAttr = string(ExchangePlugin_Import::FILE_PATH_ID());
    aFilePath = aFilePathAttr->value();
    // get the document where to import
    aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
    aTargetAttr = integer(TARGET_PART_ID());
  }

  if (aFilePath.empty()) {
      setError("File path is empty.");
      return;
  }
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc = findDocument(aSession->moduleDocument(),
      Locale::Convert::toWString(aPartsAttr->value(aTargetAttr->value())));

  if (aDoc.get()) {
    FeaturePtr aImportFeature = aDoc->addFeature(ExchangePlugin_ImportFeature::ID());
    DataPtr aData = aImportFeature->data();
    AttributeStringPtr aPathAttr;
    if (aFormat == "STEP" || aFormat == "STP")
    {
      aPathAttr = aData->string(ExchangePlugin_ImportFeature::STEP_FILE_PATH_ID());
    }else
    {
      aPathAttr = aData->string(ExchangePlugin_ImportFeature::FILE_PATH_ID());
    }

    AttributeStringPtr aImportTypeAttr =
        aData->string(ExchangePlugin_ImportFeature::IMPORT_TYPE_ID());

    aData->boolean(ExchangePlugin_ImportFeature::STEP_MATERIALS_ID())
        ->setValue(boolean(ExchangePlugin_Import::STEP_MATERIALS_ID())->value());
    aData->boolean(ExchangePlugin_ImportFeature::STEP_COLORS_ID())
        ->setValue(boolean(ExchangePlugin_Import::STEP_COLORS_ID())->value());
    aData->boolean(ExchangePlugin_ImportFeature::STEP_SCALE_INTER_UNITS_ID())
        ->setValue(boolean(ExchangePlugin_Import::STEP_SCALE_INTER_UNITS_ID())->value());

    aPathAttr->setValue(aFilePathAttr->value());
    aImportTypeAttr->setValue(aFormat);
    aImportFeature->execute();
  }
}

void ExchangePlugin_Import_Image::execute()
{
  AttributeStringPtr aFilePathAttr = string(ExchangePlugin_ImportBase::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty()) {
    setError("File path is empty.");
    return;
  }

  // get the document where to import
  AttributeStringArrayPtr aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
  AttributeIntegerPtr aTargetAttr = integer(TARGET_PART_ID());
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc = findDocument(aSession->moduleDocument(),
      Locale::Convert::toWString(aPartsAttr->value(aTargetAttr->value())));

  if (aDoc.get()) {
    FeaturePtr aImportFeature = aDoc->addFeature(ExchangePlugin_Import_ImageFeature::ID());
    DataPtr aData = aImportFeature->data();
    AttributeStringPtr aPathAttr =
        aData->string(ExchangePlugin_Import_ImageFeature::FILE_PATH_ID());
    aPathAttr->setValue(aFilePathAttr->value());
    aImportFeature->execute();
  }
}
void ExchangePlugin_Import::attributeChanged(const std::string& theID)
{
  AttributeStringPtr aFilePathAttr;
  AttributeStringArrayPtr aPartsAttr;
  AttributeIntegerPtr aTargetAttr;

  if (theID == FILE_PATH_ID() ||theID == STEP_FILE_PATH_ID() ) {
    aFilePathAttr = string(FILE_PATH_ID());
    if (theID == FILE_PATH_ID() && aFilePathAttr->value().empty())
      return;
    aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
    aTargetAttr = integer(TARGET_PART_ID());

    updatePart(aPartsAttr, aTargetAttr);

    aFilePathAttr = string(STEP_FILE_PATH_ID());
    if (theID == STEP_FILE_PATH_ID() && aFilePathAttr->value().empty())
      return;

    aPartsAttr = stringArray(STEP_TARGET_PARTS_LIST_ID());
    aTargetAttr = integer(STEP_TARGET_PART_ID());
    updatePart(aPartsAttr, aTargetAttr);
   }
}

void ExchangePlugin_Import_Image::attributeChanged(const std::string& theID)
{
  if (theID == FILE_PATH_ID()) {
    AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
    if (aFilePathAttr->value().empty())
      return;

    AttributeStringArrayPtr aPartsAttr = stringArray(TARGET_PARTS_LIST_ID());
    AttributeIntegerPtr aTargetAttr = integer(TARGET_PART_ID());
    updatePart(aPartsAttr, aTargetAttr);
  }
}

void ExchangePlugin_ImportBase::updatePart(AttributeStringArrayPtr& aPartsAttr,
                                       AttributeIntegerPtr& aTargetAttr)
{

  // update the list of target parts
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc = document();
  bool isPartSet = aDoc == aSession->moduleDocument();
  if (isPartSet) {
    std::list<std::wstring> anAcceptedValues;
    anAcceptedValues.push_back(THE_NEW_PART_STR);

    // append names of all parts
    std::list<FeaturePtr> aSubFeatures = aDoc->allFeatures();
    for (std::list<FeaturePtr>::iterator aFIt = aSubFeatures.begin();
         aFIt != aSubFeatures.end(); ++aFIt) {
      if ((*aFIt)->getKind() == PartSetPlugin_Part::ID())
        anAcceptedValues.push_back((*aFIt)->name());
    }

    if ((size_t)aPartsAttr->size() != anAcceptedValues.size())
      aTargetAttr->setValue(0);

    aPartsAttr->setSize((int)anAcceptedValues.size());
    std::list<std::wstring>::iterator anIt = anAcceptedValues.begin();
    for (int anInd = 0; anIt != anAcceptedValues.end(); ++anIt, ++anInd)
      aPartsAttr->setValue(anInd, Locale::Convert::toString(*anIt));
  }
  else {
    // keep only the name of the current part
    if (aPartsAttr->size() == 0) {
      FeaturePtr aPartFeature = ModelAPI_Tools::findPartFeature(aSession->moduleDocument(), aDoc);

      aPartsAttr->setSize(1);
      aPartsAttr->setValue(0, Locale::Convert::toString(aPartFeature->name()));
      aTargetAttr->setValue(0);
    }
  }
}

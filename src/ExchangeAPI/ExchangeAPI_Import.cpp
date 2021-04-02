// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "ExchangeAPI_Import.h"
//--------------------------------------------------------------------------------------
#include <ExchangePlugin_ImportPart.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <GeomAlgoAPI_Tools.h>
//--------------------------------------------------------------------------------------
#include <algorithm>

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFilePath(theFilePath);
}

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath,
    const bool theScalInterUnits,
    const bool theMaterials,
    const bool theColor)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setParameters(theFilePath, theScalInterUnits, theMaterials, theColor);
}

ExchangeAPI_Import::~ExchangeAPI_Import()
{

}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::setParameters(const std::string & theFilePath,
                                       const bool theScalInterUnits,
                                       const bool theMaterials,
                                       const bool theColor)
{
  fillAttribute(theFilePath, mystepFilePath);
  fillAttribute("STEP", myimportType);
  fillAttribute(theScalInterUnits, myscalInterUnits);
  fillAttribute(theMaterials,mymaterials);
  fillAttribute(theColor,mycolors);
  execute();
}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::setFilePath(const std::string & theFilePath)
{

  std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFilePath);
  if (anExtension == "STEP" || anExtension == "STP") {
    setParameters(theFilePath,true,false,false);
  } else {
    fillAttribute(theFilePath, myfilePath);
    fillAttribute(anExtension, myimportType);
    execute();
  }
}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  AttributeStringPtr aImportTypeAttr =
                    aBase->string(ExchangePlugin_ImportFeature::IMPORT_TYPE_ID());
  std::string aFormat = aImportTypeAttr->value();
  std::string aFilePath;
  if (aFormat == "STEP" || aFormat == "STP")
  {
    aFilePath = aBase->string(ExchangePlugin_ImportFeature::STEP_FILE_PATH_ID())->value();
  } else {
    aFilePath = aBase->string(ExchangePlugin_ImportFeature::FILE_PATH_ID())->value();
  }

  std::string aFrom = "\\";
  std::string aTo = "\\\\";
  for(std::size_t aPos = aFilePath.find(aFrom);
      aPos != std::string::npos;
      aPos = aFilePath.find(aFrom, aPos)) {
    aFilePath.replace(aPos, aFrom.size(), aTo);
    aPos += aTo.size();
  }
  std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(aFilePath);
  if (anExtension == "STP" || anExtension == "STEP"){
      theDumper << aBase << " = model.addImportSTEP(" << aPartName << ", \""
                << aFilePath << "\"" ;

      theDumper << ", " << scalInterUnits()->value()
                << ", " << materials()->value()
                << ", " << colors()->value() << ")"<< std::endl;
  } else {
      theDumper << aBase << " = model.addImport(" << aPartName << ", \""
            << aFilePath << "\")" << std::endl;
  }

  // to make import have results
  theDumper << "model.do()" << std::endl;

  CompositeFeaturePtr aCompositeFeature =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBase);
  if (aCompositeFeature.get()) {
    int aNbOfSubs = aCompositeFeature->numberOfSubs();
    for(int anIndex = 0; anIndex < aNbOfSubs; ++anIndex) {
      std::string aSubFeatureGet =
        theDumper.name(aBase) + ".subFeature(" + std::to_string((long long)anIndex) + ")";
      theDumper.dumpSubFeatureNameAndColor(aSubFeatureGet, aCompositeFeature->subFeature(anIndex));
    }
  }
}

//--------------------------------------------------------------------------------------
ImportPtr addImport(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Import::ID());
  return ImportPtr(new ExchangeAPI_Import(aFeature, theFilePath));
}

ImportPtr addImportSTEP(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath,
    const bool theScalInterUnits,
    const bool theMaterials,
    const bool theColor )
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Import::ID());
  return ImportPtr(new ExchangeAPI_Import(aFeature, theFilePath,
                                          theScalInterUnits, theMaterials, theColor));
}

void importPart(const std::shared_ptr<ModelAPI_Document> & thePart,
                const std::string & theFilePath,
                const ModelHighAPI_Reference & theAfterThis)
{
  static const bool THE_VISIBLE_FEATURE = false;
  FeaturePtr aCurrentFeature;
  if (theAfterThis.feature()) {
    aCurrentFeature = thePart->currentFeature(THE_VISIBLE_FEATURE);
    thePart->setCurrentFeature(theAfterThis.feature(), THE_VISIBLE_FEATURE);
  }

  FeaturePtr aFeature = thePart->addFeature(ExchangePlugin_ImportPart::ID());
  aFeature->string(ExchangePlugin_ImportPart::FILE_PATH_ID())->setValue(theFilePath);

  // specify the ID of selected document
  int aTargetPartIndex = 0;
  SessionPtr aSession = ModelAPI_Session::get();
  if (aSession->moduleDocument() == thePart) {
    // Importing to PartSet has 2 choices: import directly to PartSet (if possible)
    // or create a new part. Because then importing to existing part the document
    // has to be specified explicitly.
    // As a result, parse the list of possible target documents and generate new part
    // if the import document is not applicable on PartSet level
    // (there is no 'PartSet' in the list of applicable documents).
    AttributeStringArrayPtr aDocsList =
        aFeature->stringArray(ExchangePlugin_ImportPart::TARGET_PARTS_LIST_ID());
    if (aDocsList->size() > 1 && aDocsList->value(1) == "PartSet")
      aTargetPartIndex = 1;
  }
  aFeature->integer(ExchangePlugin_ImportPart::TARGET_PART_ID())->setValue(aTargetPartIndex);

  // restart transaction to execute and delete the macro-feature
  apply();

  // restore current feature
  if (aCurrentFeature)
    thePart->setCurrentFeature(aCurrentFeature, THE_VISIBLE_FEATURE);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

ExchangeAPI_Import_Image::ExchangeAPI_Import_Image(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ExchangeAPI_Import_Image::ExchangeAPI_Import_Image(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFilePath(theFilePath);
}

void ExchangeAPI_Import_Image::setFilePath(const std::string & theFilePath)
{
  fillAttribute(theFilePath, myfilePath);
  execute();
}

ImportImagePtr addImportImage(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Import_Image::ID());
  return ImportImagePtr(new ExchangeAPI_Import_Image(aFeature, theFilePath));
}

void ExchangeAPI_Import_Image::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  std::string aFilePath =
      aBase->string(ExchangePlugin_Import_ImageFeature::FILE_PATH_ID())->value();

  theDumper << aBase << " = model.addImportImage(" << aPartName << ", \""
            << aFilePath << "\")" << std::endl;

  // to make import have results
  theDumper << "model.do()" << std::endl;
}

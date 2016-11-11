// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// Name   : ExchangeAPI_Import.cpp
// Purpose:
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ExchangeAPI_Import.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>
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

ExchangeAPI_Import::~ExchangeAPI_Import()
{

}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::setFilePath(const std::string & theFilePath)
{
  fillAttribute(theFilePath, myfilePath);

  execute();
}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  std::string aFilePath = aBase->string(ExchangePlugin_ImportFeature::FILE_PATH_ID())->value();
  std::string aFrom = "\\";
  std::string aTo = "\\\\";
  for(std::size_t aPos = aFilePath.find(aFrom);
      aPos != std::string::npos;
      aPos = aFilePath.find(aFrom, aPos)) {
    aFilePath.replace(aPos, aFrom.size(), aTo);
    aPos += aTo.size();
  }

  theDumper << aBase << " = model.addImport(" << aPartName << ", \""
            << aFilePath << "\")" << std::endl;
  // to make import have results
  theDumper << "model.do()" << std::endl;

  CompositeFeaturePtr aCompositeFeature =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBase);
  if(aCompositeFeature.get()) {
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

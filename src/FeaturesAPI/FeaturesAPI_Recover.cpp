// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Recover.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Recover.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//=================================================================================================
FeaturesAPI_Recover::FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
FeaturesAPI_Recover::FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Reference& theBaseFeature,
  const std::list<ModelHighAPI_Selection>& theRecoveredList, const bool thePersistent)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBaseFeature(theBaseFeature);
    setRecoveredList(theRecoveredList);
  }
}

//=================================================================================================
FeaturesAPI_Recover::~FeaturesAPI_Recover()
{}

//=================================================================================================
void FeaturesAPI_Recover::setBaseFeature(const ModelHighAPI_Reference& theBaseFeature)
{

  fillAttribute(theBaseFeature.feature(), mybaseFeature);
  execute();
}

//=================================================================================================
void FeaturesAPI_Recover::setRecoveredList(const std::list<ModelHighAPI_Selection>& theRecoverList)
{
  fillAttribute(theRecoverList, myrecoveredList);
  execute();
}

//==================================================================================================
void FeaturesAPI_Recover::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrBaseFeature =
    aBase->reference(FeaturesPlugin_Recover::BASE_FEATURE());
  AttributeRefListPtr anAttrRecoveredEntities =
    aBase->reflist(FeaturesPlugin_Recover::RECOVERED_ENTITIES());

  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttrBaseFeature->value());

  theDumper << aBase << " = model.addRecover(" << aDocName << ", "
            << aFeature << ", " << anAttrRecoveredEntities << ")" << std::endl;
}

//=================================================================================================
RecoverPtr addRecover(const std::shared_ptr<ModelAPI_Document>& thePart,
  const ModelHighAPI_Reference& theBaseFeature,
  const std::list<ModelHighAPI_Selection>& theRecoveredList, const bool thePersistent)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Recover::ID());
  return RecoverPtr(new FeaturesAPI_Recover(
    aFeature, theBaseFeature, theRecoveredList, thePersistent));
}

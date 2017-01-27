// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Scale.cpp
// Created:     24 Jan 2017
// Author:      Clarisse Genrault (CEA)

#include "FeaturesAPI_Scale.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                                     const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Double& theScaleFactor)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theCenterPoint, centerPoint());
    setScaleFactor(theScaleFactor);
  }
}

//==================================================================================================
FeaturesAPI_Scale::~FeaturesAPI_Scale()
{
}

//==================================================================================================
void FeaturesAPI_Scale::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setCenterPoint(const ModelHighAPI_Selection& theCenterPoint)
{
  fillAttribute(theCenterPoint, centerPoint());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setScaleFactor(const ModelHighAPI_Double& theScaleFactor)
{
  fillAttribute(theScaleFactor, scaleFactor());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addScale(" << aDocName << ", " << anAttrObjects;

  AttributeSelectionPtr anAttrPoint =
    aBase->selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  theDumper << " , " << anAttrPoint;

  AttributeDoublePtr anAttrScaleFactor =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_ID());
    theDumper << ", " << anAttrScaleFactor;

  theDumper << ")" << std::endl;
}

//==================================================================================================
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactor)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Scale::ID());
  return ScalePtr(new FeaturesAPI_Scale(aFeature, theMainObjects, theCenterPoint, theScaleFactor));
}
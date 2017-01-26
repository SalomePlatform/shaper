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
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                                     const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Double& theScaleFactorX,
                                     const ModelHighAPI_Double& theScaleFactorY,
                                     const ModelHighAPI_Double& theScaleFactorZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theCenterPoint, centerPoint());
    setDimensions(theScaleFactorX, theScaleFactorY, theScaleFactorZ);
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
  fillAttribute(FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR(), creationMethod());
  fillAttribute(theScaleFactor, scaleFactor());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setDimensions(const ModelHighAPI_Double& theScaleFactorX,
                                      const ModelHighAPI_Double& theScaleFactorY,
                                      const ModelHighAPI_Double& theScaleFactorZ)
{
  fillAttribute(FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
  fillAttribute(theScaleFactorX, scaleFactorX());
  fillAttribute(theScaleFactorY, scaleFactorY());
  fillAttribute(theScaleFactorZ, scaleFactorZ());
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

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Scale::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR()) {
    AttributeDoublePtr anAttrScaleFactor =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_ID());
    theDumper << ", " << anAttrScaleFactor;
  } else if (aCreationMethod == FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrScaleFactorX =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID());
    AttributeDoublePtr anAttrScaleFactorY =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID());
    AttributeDoublePtr anAttrScaleFactorZ =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID());
    theDumper << ", " << anAttrScaleFactorX << " , " << anAttrScaleFactorY;
    theDumper << ", " << anAttrScaleFactorZ;
  }

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

//==================================================================================================
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactorX,
                  const ModelHighAPI_Double& theScaleFactorY,
                  const ModelHighAPI_Double& theScaleFactorZ)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Scale::ID());
  return ScalePtr(new FeaturesAPI_Scale(aFeature, theMainObjects, theCenterPoint,
                  theScaleFactorX, theScaleFactorY, theScaleFactorZ));
}
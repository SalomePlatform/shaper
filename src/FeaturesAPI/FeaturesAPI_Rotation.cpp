// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Rotation.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Rotation.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                                           const ModelHighAPI_Selection& theAxisObject,
                                           const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theAxisObject, myaxisObject);
    setAngle(theAngle);
  }
}

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                                           const ModelHighAPI_Selection& theCenterPoint,
                                           const ModelHighAPI_Selection& theStartPoint,
                                           const ModelHighAPI_Selection& theEndPoint)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    setPoints(theCenterPoint, theStartPoint, theEndPoint);
  }
}

//==================================================================================================
FeaturesAPI_Rotation::~FeaturesAPI_Rotation()
{

}

//==================================================================================================
void FeaturesAPI_Rotation::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAxisObject(const ModelHighAPI_Selection& theAxisObject)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE(), mycreationMethod);
  fillAttribute(theAxisObject, myaxisObject);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE(), mycreationMethod);
  fillAttribute(theAngle, myangle);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setPoints(const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Selection& theStartPoint,
                                     const ModelHighAPI_Selection& theEndPoint)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_THREE_POINTS(), mycreationMethod);
  fillAttribute(theCenterPoint, centerPoint());
  fillAttribute(theStartPoint, startPoint());
  fillAttribute(theEndPoint, endPoint());

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Rotation::OBJECTS_LIST_ID());

  theDumper << aBase << " = model.addRotation(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Rotation::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE()) {
    AttributeSelectionPtr anAttrAxis = aBase->selection(FeaturesPlugin_Rotation::AXIS_OBJECT_ID());
    AttributeDoublePtr anAttrAngle = aBase->real(FeaturesPlugin_Rotation::ANGLE_ID());
    theDumper << ", " << anAttrAxis << ", " << anAttrAngle;
  } else if (aCreationMethod == FeaturesPlugin_Rotation::CREATION_METHOD_BY_THREE_POINTS()) {
    AttributeSelectionPtr anAttrCenterPoint =
      aBase->selection(FeaturesPlugin_Rotation::CENTER_POINT_ID());
    AttributeSelectionPtr anAttrStartPoint =
      aBase->selection(FeaturesPlugin_Rotation::START_POINT_ID());
    AttributeSelectionPtr anAttrEndPoint =
      aBase->selection(FeaturesPlugin_Rotation::END_POINT_ID());
    theDumper << ", " << anAttrCenterPoint << ", " << anAttrStartPoint << ", " << anAttrEndPoint;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
RotationPtr addRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                        const ModelHighAPI_Selection& theAxisObject,
                        const ModelHighAPI_Double& theAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Rotation::ID());
  return RotationPtr(new FeaturesAPI_Rotation(aFeature, theMainObjects, theAxisObject, theAngle));
}

//==================================================================================================
RotationPtr addRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                        const ModelHighAPI_Selection& theCenterPoint,
                        const ModelHighAPI_Selection& theStartPoint,
                        const ModelHighAPI_Selection& theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Rotation::ID());
  return RotationPtr(new FeaturesAPI_Rotation(aFeature, theMainObjects, theCenterPoint,
                                              theStartPoint, theEndPoint));
}

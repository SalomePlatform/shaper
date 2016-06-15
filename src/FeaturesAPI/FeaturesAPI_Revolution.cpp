// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Revolution.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Revolution.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                               const ModelHighAPI_Selection& theAxis,
                                               const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    setAngles(theAngle, ModelHighAPI_Double());
  }
}

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                               const ModelHighAPI_Selection& theAxis,
                                               const ModelHighAPI_Double& theToAngle,
                                               const ModelHighAPI_Double& theFromAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    setAngles(theToAngle, theFromAngle);
  }
}

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                               const ModelHighAPI_Selection& theAxis,
                                               const ModelHighAPI_Selection& theToObject,
                                               const ModelHighAPI_Double& theToOffset,
                                               const ModelHighAPI_Selection& theFromObject,
                                               const ModelHighAPI_Double& theFromOffset)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
  }
}

//==================================================================================================
FeaturesAPI_Revolution::~FeaturesAPI_Revolution()
{

}

//==================================================================================================
void FeaturesAPI_Revolution::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAxis(const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theAxis, myaxis);

  execute();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAngles(const ModelHighAPI_Double& theToAngle,
                                       const ModelHighAPI_Double& theFromAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theToAngle, mytoAngle);
  fillAttribute(theFromAngle, myfromAngle);

  execute();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theAngle, mytoAngle);
  fillAttribute(ModelHighAPI_Double(), myfromAngle);

  execute();
}

//==================================================================================================
void FeaturesAPI_Revolution::setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                                                const ModelHighAPI_Double& theToOffset,
                                                const ModelHighAPI_Selection& theFromObject,
                                                const ModelHighAPI_Double& theFromOffset)
{
  fillAttribute("ByPlanesAndOffsets", mycreationMethod);
  fillAttribute(theToObject, mytoObject);
  fillAttribute(theToOffset, mytoOffset);
  fillAttribute(theFromObject, myfromObject);
  fillAttribute(theFromOffset, myfromOffset);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Double& theAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Revolution::ID());
  return RevolutionPtr(new FeaturesAPI_Revolution(aFeature, theBaseObjects, theAxis, theAngle));
}

//==================================================================================================
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Double& theToAngle,
                            const ModelHighAPI_Double& theFromAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Revolution::ID());
  return RevolutionPtr(new FeaturesAPI_Revolution(aFeature,
                                                theBaseObjects,
                                                theAxis,
                                                theToAngle,
                                                theFromAngle));
}

//==================================================================================================
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Selection& theToObject,
                            const ModelHighAPI_Double& theToOffset,
                            const ModelHighAPI_Selection& theFromObject,
                            const ModelHighAPI_Double& theFromOffset)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Revolution::ID());
  return RevolutionPtr(new FeaturesAPI_Revolution(aFeature,
                                                theBaseObjects,
                                                theAxis,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset));
}

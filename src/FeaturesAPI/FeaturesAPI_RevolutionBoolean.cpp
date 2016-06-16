// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_RevolutionBoolean.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_RevolutionBoolean.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_RevolutionBoolean::FeaturesAPI_RevolutionBoolean(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
}

//==================================================================================================
FeaturesAPI_RevolutionBoolean::~FeaturesAPI_RevolutionBoolean()
{
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAxis(const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theAxis, myaxis);

  execute();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAngles(const ModelHighAPI_Double& theToAngle,
                                              const ModelHighAPI_Double& theFromAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theToAngle, mytoAngle);
  fillAttribute(theFromAngle, myfromAngle);

  execute();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theAngle, mytoAngle);
  fillAttribute(ModelHighAPI_Double(), myfromAngle);

  execute();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                                                        const ModelHighAPI_Double& theToOffset,
                                                        const ModelHighAPI_Selection& theFromObject,
                                                        const ModelHighAPI_Double& theFromOffset)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_PLANES(), mycreationMethod);
  fillAttribute(theToObject, mytoObject);
  fillAttribute(theToOffset, mytoOffset);
  fillAttribute(theFromObject, myfromObject);
  fillAttribute(theFromOffset, myfromOffset);

  execute();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setBooleanObjects(const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  fillAttribute(theBooleanObjects, mybooleanObjects);

  execute();
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theAxis,
                                                     const ModelHighAPI_Double& theSize,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theSize, mytoAngle);
    fillAttribute(ModelHighAPI_Double(), myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theAxis,
                                                     const ModelHighAPI_Double& theToAngle,
                                                     const ModelHighAPI_Double& theFromAngle,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theToAngle, mytoAngle);
    fillAttribute(theFromAngle, myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theAxis,
                                                     const ModelHighAPI_Selection& theToObject,
                                                     const ModelHighAPI_Double& theToOffset,
                                                     const ModelHighAPI_Selection& theFromObject,
                                                     const ModelHighAPI_Double& theFromOffset,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
  return RevolutionCutPtr(new FeaturesAPI_RevolutionCut(aFeature, theBaseObjects, theAxis, theSize, theBooleanObjects));
}

//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theToAngle,
                                  const ModelHighAPI_Double& theFromAngle,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
  return RevolutionCutPtr(new FeaturesAPI_RevolutionCut(aFeature,
                                                      theBaseObjects,
                                                      theAxis,
                                                      theToAngle,
                                                      theFromAngle,
                                                      theBooleanObjects));
}

//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
  return RevolutionCutPtr(new FeaturesAPI_RevolutionCut(aFeature,
                                                      theBaseObjects,
                                                      theAxis,
                                                      theToObject,
                                                      theToOffset,
                                                      theFromObject,
                                                      theFromOffset,
                                                      theBooleanObjects));
}


//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                       const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                       const ModelHighAPI_Selection& theAxis,
                                                       const ModelHighAPI_Double& theSize,
                                                       const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theSize, mytoAngle);
    fillAttribute(ModelHighAPI_Double(), myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                       const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                       const ModelHighAPI_Selection& theAxis,
                                                       const ModelHighAPI_Double& theToAngle,
                                                       const ModelHighAPI_Double& theFromAngle,
                                                       const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theToAngle, mytoAngle);
    fillAttribute(theFromAngle, myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                       const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                       const ModelHighAPI_Selection& theAxis,
                                                       const ModelHighAPI_Selection& theToObject,
                                                       const ModelHighAPI_Double& theToOffset,
                                                       const ModelHighAPI_Selection& theFromObject,
                                                       const ModelHighAPI_Double& theFromOffset,
                                                       const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature, theBaseObjects, theAxis, theSize, theBooleanObjects));
}

//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theToAngle,
                                    const ModelHighAPI_Double& theFromAngle,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature,
                                                        theBaseObjects,
                                                        theAxis,
                                                        theToAngle,
                                                        theFromAngle,
                                                        theBooleanObjects));
}

//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Selection& theToObject,
                                    const ModelHighAPI_Double& theToOffset,
                                    const ModelHighAPI_Selection& theFromObject,
                                    const ModelHighAPI_Double& theFromOffset,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature,
                                                        theBaseObjects,
                                                        theAxis,
                                                        theToObject,
                                                        theToOffset,
                                                        theFromObject,
                                                        theFromOffset,
                                                        theBooleanObjects));
}

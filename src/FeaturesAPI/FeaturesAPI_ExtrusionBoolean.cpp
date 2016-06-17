// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_ExtrusionBoolean.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_ExtrusionBoolean.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_ExtrusionBoolean::FeaturesAPI_ExtrusionBoolean(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
}

//==================================================================================================
FeaturesAPI_ExtrusionBoolean::~FeaturesAPI_ExtrusionBoolean()
{
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setDirection(const ModelHighAPI_Selection& theDirection)
{
  fillAttribute(theDirection, mydirection);

  execute();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setSizes(const ModelHighAPI_Double& theToSize,
                                            const ModelHighAPI_Double& theFromSize)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
  fillAttribute(theToSize, mytoSize);
  fillAttribute(theFromSize, myfromSize);

  execute();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setSize(const ModelHighAPI_Double& theSize)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
  fillAttribute(theSize, mytoSize);
  fillAttribute(ModelHighAPI_Double(), myfromSize);

  execute();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                                                       const ModelHighAPI_Double& theToOffset,
                                                       const ModelHighAPI_Selection& theFromObject,
                                                       const ModelHighAPI_Double& theFromOffset)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES(), mycreationMethod);
  fillAttribute(theToObject, mytoObject);
  fillAttribute(theToOffset, mytoOffset);
  fillAttribute(theFromObject, myfromObject);
  fillAttribute(theFromOffset, myfromOffset);

  execute();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setBooleanObjects(const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  fillAttribute(theBooleanObjects, mybooleanObjects);

  execute();
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Double& theSize,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theSize, mytoSize);
    fillAttribute(ModelHighAPI_Double(), myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Selection& theDirection,
                                                   const ModelHighAPI_Double& theSize,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theSize, mytoSize);
    fillAttribute(ModelHighAPI_Double(), myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Double& theToSize,
                                                   const ModelHighAPI_Double& theFromSize,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theToSize, mytoSize);
    fillAttribute(theFromSize, myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Selection& theDirection,
                                                   const ModelHighAPI_Double& theToSize,
                                                   const ModelHighAPI_Double& theFromSize,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theToSize, mytoSize);
    fillAttribute(theFromSize, myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Selection& theToObject,
                                                   const ModelHighAPI_Double& theToOffset,
                                                   const ModelHighAPI_Selection& theFromObject,
                                                   const ModelHighAPI_Double& theFromOffset,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                   const ModelHighAPI_Selection& theDirection,
                                                   const ModelHighAPI_Selection& theToObject,
                                                   const ModelHighAPI_Double& theToOffset,
                                                   const ModelHighAPI_Selection& theFromObject,
                                                   const ModelHighAPI_Double& theFromOffset,
                                                   const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects, theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects, theDirection, theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects, theToSize, theFromSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature,
                                                      theBaseObjects,
                                                      theDirection,
                                                      theToSize,
                                                      theFromSize,
                                                      theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theToObject,
                                const ModelHighAPI_Double& theToOffset,
                                const ModelHighAPI_Selection& theFromObject,
                                const ModelHighAPI_Double& theFromOffset,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature,
                                                      theBaseObjects,
                                                      theToObject,
                                                      theToOffset,
                                                      theFromObject,
                                                      theFromOffset,
                                                      theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Selection& theToObject,
                                const ModelHighAPI_Double& theToOffset,
                                const ModelHighAPI_Selection& theFromObject,
                                const ModelHighAPI_Double& theFromOffset,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature,
                                                      theBaseObjects,
                                                      theDirection,
                                                      theToObject,
                                                      theToOffset,
                                                      theFromObject,
                                                      theFromOffset,
                                                      theBooleanObjects));
}


//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Double& theSize,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theSize, mytoSize);
    fillAttribute(ModelHighAPI_Double(), myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theDirection,
                                                     const ModelHighAPI_Double& theSize,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theSize, mytoSize);
    fillAttribute(ModelHighAPI_Double(), myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Double& theToSize,
                                                     const ModelHighAPI_Double& theFromSize,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theToSize, mytoSize);
    fillAttribute(theFromSize, myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theDirection,
                                                     const ModelHighAPI_Double& theToSize,
                                                     const ModelHighAPI_Double& theFromSize,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theToSize, mytoSize);
    fillAttribute(theFromSize, myfromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theToObject,
                                                     const ModelHighAPI_Double& theToOffset,
                                                     const ModelHighAPI_Selection& theFromObject,
                                                     const ModelHighAPI_Double& theFromOffset,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                                     const ModelHighAPI_Selection& theDirection,
                                                     const ModelHighAPI_Selection& theToObject,
                                                     const ModelHighAPI_Double& theToOffset,
                                                     const ModelHighAPI_Selection& theFromObject,
                                                     const ModelHighAPI_Double& theFromOffset,
                                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects, theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects, theDirection, theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects, theToSize, theFromSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature,
                                                        theBaseObjects,
                                                        theDirection,
                                                        theToSize,
                                                        theFromSize,
                                                        theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature,
                                                        theBaseObjects,
                                                        theToObject,
                                                        theToOffset,
                                                        theFromObject,
                                                        theFromOffset,
                                                        theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature,
                                                        theBaseObjects,
                                                        theDirection,
                                                        theToObject,
                                                        theToOffset,
                                                        theFromObject,
                                                        theFromOffset,
                                                        theBooleanObjects));
}

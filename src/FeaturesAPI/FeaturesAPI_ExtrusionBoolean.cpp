// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_ExtrusionBoolean.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_ExtrusionBoolean.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
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
void FeaturesAPI_ExtrusionBoolean::setNestedSketch(const ModelHighAPI_Reference& theSketch)
{
  mysketch->setValue(theSketch.feature());
  mybaseObjects->clear();
  mybaseObjects->append(theSketch.feature()->firstResult(), GeomShapePtr());

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mysketch->setValue(ObjectPtr());
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setDirection(const ModelHighAPI_Selection& theDirection)
{
  fillAttribute(theDirection, mydirection);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setSizes(const ModelHighAPI_Double& theToSize,
                                            const ModelHighAPI_Double& theFromSize)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
  fillAttribute(theToSize, mytoSize);
  fillAttribute(theFromSize, myfromSize);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setSize(const ModelHighAPI_Double& theSize)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
  fillAttribute(theSize, mytoSize);
  fillAttribute(ModelHighAPI_Double(), myfromSize);

  execIfBaseNotEmpty();
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

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::setBooleanObjects(const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  fillAttribute(theBooleanObjects, mybooleanObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrSketch = aBase->reference(FeaturesPlugin_Extrusion::SKETCH_ID());
  AttributeSelectionListPtr anAttrObjects = aBase->selectionList(FeaturesPlugin_Extrusion::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrDirection = aBase->selection(FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID());

  theDumper << aBase << " = model.addExtrusion";
  if(aBase->getKind() == FeaturesPlugin_ExtrusionCut::ID()) {
    theDumper << "Cut";
  } else if(aBase->getKind() == FeaturesPlugin_ExtrusionFuse::ID()) {
    theDumper << "Fuse";
  }
  theDumper << "(" << aDocName << ", ";
  anAttrSketch->isInitialized() ? theDumper << "[]" : theDumper << anAttrObjects;
  theDumper << ", " << anAttrDirection;

  std::string aCreationMethod = aBase->string(FeaturesPlugin_Extrusion::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES()) {
    AttributeDoublePtr anAttrToSize = aBase->real(FeaturesPlugin_Extrusion::TO_SIZE_ID());
    AttributeDoublePtr anAttrFromSize = aBase->real(FeaturesPlugin_Extrusion::FROM_SIZE_ID());

    theDumper << ", " << anAttrToSize << ", " << anAttrFromSize;
  } else if(aCreationMethod == FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES()) {
    AttributeSelectionPtr anAttrToObject = aBase->selection(FeaturesPlugin_Extrusion::TO_OBJECT_ID());
    AttributeDoublePtr anAttrToOffset = aBase->real(FeaturesPlugin_Extrusion::TO_OFFSET_ID());
    AttributeSelectionPtr anAttrFromObject = aBase->selection(FeaturesPlugin_Extrusion::FROM_OBJECT_ID());
    AttributeDoublePtr anAttrFromOffset = aBase->real(FeaturesPlugin_Extrusion::FROM_OFFSET_ID());

    theDumper << ", " << anAttrToObject << ", " << anAttrToOffset << ", " << anAttrFromObject << ", " << anAttrFromOffset;
  }

  AttributeSelectionListPtr anAttrBoolObjects = aBase->selectionList(FeaturesPlugin_CompositeBoolean::OBJECTS_ID());
  theDumper << ", " << anAttrBoolObjects << ")" << std::endl;

  if(anAttrSketch->isInitialized()) {
    theDumper << aBase << ".setNestedSketch(" << anAttrSketch << ")" << std::endl;
  }
}

//==================================================================================================
void FeaturesAPI_ExtrusionBoolean::execIfBaseNotEmpty()
{
  if(mybaseObjects->size() > 0) {
    execute();
  }
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES(), mycreationMethod);
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
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES(), mycreationMethod);
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

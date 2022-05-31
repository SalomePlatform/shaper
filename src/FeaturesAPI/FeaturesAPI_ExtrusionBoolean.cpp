// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_ExtrusionBoolean.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_ExtrusionBoolean::FeaturesAPI_ExtrusionBoolean(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
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

  // To make Sketch feature execute and subfeatures execute.
  feature()->document()->setCurrentFeature(feature(), false);

  // to inform that the history is updated due to the sketch moved under the composite feature
  if (theSketch.feature().get()) {
    theSketch.feature()->document()->updateHistory(ModelAPI_Feature::group());
    if (theSketch.feature()->firstResult().get())
      theSketch.feature()->firstResult()->setDisplayed(false);
  }
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
  setSizes(theSize, ModelHighAPI_Double());
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
void FeaturesAPI_ExtrusionBoolean::setBooleanObjects(
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
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
  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Extrusion::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrDirection =
    aBase->selection(FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID());

  theDumper << aBase << " = model.addExtrusion";
  if(aBase->getKind() == FeaturesPlugin_ExtrusionCut::ID()) {
    theDumper << "Cut";
  } else if(aBase->getKind() == FeaturesPlugin_ExtrusionFuse::ID()) {
    theDumper << "Fuse";
  }
  theDumper << "(" << aDocName << ", ";
  anAttrSketch->isInitialized() ? theDumper << "[]" : theDumper << anAttrObjects;
  theDumper << ", " << anAttrDirection;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Extrusion::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES()) {
    AttributeDoublePtr anAttrToSize = aBase->real(FeaturesPlugin_Extrusion::TO_SIZE_ID());
    AttributeDoublePtr anAttrFromSize = aBase->real(FeaturesPlugin_Extrusion::FROM_SIZE_ID());

    theDumper << ", " << anAttrToSize << ", " << anAttrFromSize;
  } else if(aCreationMethod == FeaturesPlugin_Extrusion::CREATION_METHOD_BY_PLANES()) {
    AttributeSelectionPtr anAttrToObject =
      aBase->selection(FeaturesPlugin_Extrusion::TO_OBJECT_ID());
    AttributeDoublePtr anAttrToOffset = aBase->real(FeaturesPlugin_Extrusion::TO_OFFSET_ID());
    AttributeSelectionPtr anAttrFromObject =
      aBase->selection(FeaturesPlugin_Extrusion::FROM_OBJECT_ID());
    AttributeDoublePtr anAttrFromOffset = aBase->real(FeaturesPlugin_Extrusion::FROM_OFFSET_ID());

    theDumper << ", " << anAttrToObject << ", " << anAttrToOffset <<
      ", " << anAttrFromObject << ", " << anAttrFromOffset;
  } else {
    // Through all
  }

  AttributeSelectionListPtr anAttrBoolObjects =
    aBase->selectionList(FeaturesPlugin_CompositeBoolean::OBJECTS_ID());
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
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSize(theSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theDirection,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theDirection,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSize(theSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Double& theToSize,
  const ModelHighAPI_Double& theFromSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theToSize, theFromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setSizes(theToSize, theFromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionCut::FeaturesAPI_ExtrusionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects,
                                                      theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects,
                                                      theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects, theDirection,
                                                      theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects, theDirection,
                                                      theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
  return ExtrusionCutPtr(new FeaturesAPI_ExtrusionCut(aFeature, theBaseObjects,
                                                      theToSize, theFromSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionCut::ID());
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
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSize(theSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theDirection,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theDirection,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSize(theSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Double& theToSize,
  const ModelHighAPI_Double& theFromSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_ExtrusionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theToSize, theFromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setSizes(theToSize, theFromSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_ExtrusionFuse::FeaturesAPI_ExtrusionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects,
                                                        theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects,
                                                        theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects,
                                                        theDirection, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects,
                                                        theDirection, theSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature, theBaseObjects,
                                                        theToSize, theFromSize, theBooleanObjects));
}

//==================================================================================================
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_ExtrusionFuse::ID());
  return ExtrusionFusePtr(new FeaturesAPI_ExtrusionFuse(aFeature,
                                                        theBaseObjects,
                                                        theDirection,
                                                        theToObject,
                                                        theToOffset,
                                                        theFromObject,
                                                        theFromOffset,
                                                        theBooleanObjects));
}

// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_RevolutionBoolean.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_RevolutionBoolean::FeaturesAPI_RevolutionBoolean(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
}

//==================================================================================================
FeaturesAPI_RevolutionBoolean::~FeaturesAPI_RevolutionBoolean()
{
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setNestedSketch(const ModelHighAPI_Reference& theSketch)
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
void FeaturesAPI_RevolutionBoolean::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mysketch->setValue(ObjectPtr());
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAxis(const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theAxis, myaxis);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAngles(const ModelHighAPI_Double& theToAngle,
                                              const ModelHighAPI_Double& theFromAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theToAngle, mytoAngle);
  fillAttribute(theFromAngle, myfromAngle);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setAngle(const ModelHighAPI_Double& theAngle)
{
  setAngles(theAngle, ModelHighAPI_Double());
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

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::setBooleanObjects(
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  fillAttribute(theBooleanObjects, mybooleanObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrSketch =
    aBase->reference(FeaturesPlugin_Revolution::SKETCH_ID());
  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Revolution::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrAxis =
    aBase->selection(FeaturesPlugin_Revolution::AXIS_OBJECT_ID());

  theDumper << aBase << " = model.addRevolution";
  if(aBase->getKind() == FeaturesPlugin_RevolutionCut::ID()) {
    theDumper << "Cut";
  } else if(aBase->getKind() == FeaturesPlugin_RevolutionFuse::ID()) {
    theDumper << "Fuse";
  }
  theDumper << "(" << aDocName << ", ";
  anAttrSketch->isInitialized() ? theDumper << "[]" : theDumper << anAttrObjects;
  theDumper << ", " << anAttrAxis;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Revolution::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES()) {
    AttributeDoublePtr anAttrToAngle = aBase->real(FeaturesPlugin_Revolution::TO_ANGLE_ID());
    AttributeDoublePtr anAttrFromAngle = aBase->real(FeaturesPlugin_Revolution::FROM_ANGLE_ID());

    theDumper << ", " << anAttrToAngle << ", " << anAttrFromAngle;
  } else if(aCreationMethod == FeaturesPlugin_Revolution::CREATION_METHOD_BY_PLANES()) {
    AttributeSelectionPtr anAttrToObject =
      aBase->selection(FeaturesPlugin_Revolution::TO_OBJECT_ID());
    AttributeDoublePtr anAttrToOffset =
      aBase->real(FeaturesPlugin_Revolution::TO_OFFSET_ID());
    AttributeSelectionPtr anAttrFromObject =
      aBase->selection(FeaturesPlugin_Revolution::FROM_OBJECT_ID());
    AttributeDoublePtr anAttrFromOffset =
      aBase->real(FeaturesPlugin_Revolution::FROM_OFFSET_ID());

    theDumper << ", " << anAttrToObject << ", " << anAttrToOffset <<
      ", " << anAttrFromObject << ", " << anAttrFromOffset;
  } else {
    // Through All
  }

  AttributeSelectionListPtr anAttrBoolObjects =
    aBase->selectionList(FeaturesPlugin_CompositeBoolean::OBJECTS_ID());
  theDumper << ", " << anAttrBoolObjects << ")" << std::endl;

  if(anAttrSketch->isInitialized()) {
    theDumper << aBase << ".setNestedSketch(" << anAttrSketch << ")" << std::endl;
  }
}

//==================================================================================================
void FeaturesAPI_RevolutionBoolean::execIfBaseNotEmpty()
{
  if(mybaseObjects->size() > 0) {
    execute();
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    setAngle(theSize);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
    fillAttribute(theToAngle, mytoAngle);
    fillAttribute(theFromAngle, myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionCut::FeaturesAPI_RevolutionCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_PLANES(), mycreationMethod);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
  return RevolutionCutPtr(new FeaturesAPI_RevolutionCut(aFeature, theBaseObjects,
                                                        theAxis, theBooleanObjects));
}

//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
  return RevolutionCutPtr(new FeaturesAPI_RevolutionCut(aFeature, theBaseObjects,
                                                        theAxis, theSize, theBooleanObjects));
}

//==================================================================================================
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theToAngle,
                                  const ModelHighAPI_Double& theFromAngle,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionCut::ID());
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
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_THROUGH_ALL(), mycreationMethod);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Double& theSize,
  const std::list<ModelHighAPI_Selection>& theBooleanObjects)
: FeaturesAPI_RevolutionBoolean(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
    fillAttribute(theSize, mytoAngle);
    fillAttribute(ModelHighAPI_Double(), myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
    fillAttribute(theToAngle, mytoAngle);
    fillAttribute(theFromAngle, myfromAngle);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
FeaturesAPI_RevolutionFuse::FeaturesAPI_RevolutionFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_PLANES(), mycreationMethod);
    fillAttribute(theToObject, mytoObject);
    fillAttribute(theToOffset, mytoOffset);
    fillAttribute(theFromObject, myfromObject);
    fillAttribute(theFromOffset, myfromOffset);
    setBooleanObjects(theBooleanObjects);
  }
}

//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature, theBaseObjects,
                                                          theAxis, theBooleanObjects));
}

//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature, theBaseObjects,
                                                          theAxis, theSize, theBooleanObjects));
}

//==================================================================================================
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theToAngle,
                                    const ModelHighAPI_Double& theFromAngle,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
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
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesPlugin_RevolutionFuse::ID());
  return RevolutionFusePtr(new FeaturesAPI_RevolutionFuse(aFeature,
                                                        theBaseObjects,
                                                        theAxis,
                                                        theToObject,
                                                        theToOffset,
                                                        theFromObject,
                                                        theFromOffset,
                                                        theBooleanObjects));
}

// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesAPI_Extrusion.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Double& theSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theSize, ModelHighAPI_Double());
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Selection& theDirection,
                                          const ModelHighAPI_Double& theSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSizes(theSize, ModelHighAPI_Double());
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Double& theToSize,
                                          const ModelHighAPI_Double& theFromSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theToSize, theFromSize);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Selection& theDirection,
                                          const ModelHighAPI_Double& theToSize,
                                          const ModelHighAPI_Double& theFromSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSizes(theToSize, theFromSize);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Selection& theToObject,
                                          const ModelHighAPI_Double& theToOffset,
                                          const ModelHighAPI_Selection& theFromObject,
                                          const ModelHighAPI_Double& theFromOffset)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                          const ModelHighAPI_Selection& theDirection,
                                          const ModelHighAPI_Selection& theToObject,
                                          const ModelHighAPI_Double& theToOffset,
                                          const ModelHighAPI_Selection& theFromObject,
                                          const ModelHighAPI_Double& theFromOffset)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::~FeaturesAPI_Extrusion()
{
}

//==================================================================================================
void FeaturesAPI_Extrusion::setNestedSketch(const ModelHighAPI_Reference& theSketch)
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
void FeaturesAPI_Extrusion::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mysketch->setValue(ObjectPtr());
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setDirection(const ModelHighAPI_Selection& theDirection)
{
  fillAttribute(theDirection, mydirection);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setSizes(const ModelHighAPI_Double& theToSize,
                                     const ModelHighAPI_Double& theFromSize)
{
  fillAttribute(FeaturesPlugin_Extrusion::CREATION_METHOD_BY_SIZES(), mycreationMethod);
  fillAttribute(theToSize, mytoSize);
  fillAttribute(theFromSize, myfromSize);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setSize(const ModelHighAPI_Double& theSize)
{
  setSizes(theSize, ModelHighAPI_Double());
}

//==================================================================================================
void FeaturesAPI_Extrusion::setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
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
void FeaturesAPI_Extrusion::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrSketch = aBase->reference(FeaturesPlugin_Extrusion::SKETCH_ID());
  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Extrusion::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrDirection =
    aBase->selection(FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID());

  theDumper << aBase << " = model.addExtrusion(" << aDocName << ", ";
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
  }

  theDumper << ")" << std::endl;

  if(anAttrSketch->isInitialized()) {
    theDumper << aBase << ".setNestedSketch(" << anAttrSketch << ")" << std::endl;
  }
}

//==================================================================================================
void FeaturesAPI_Extrusion::execIfBaseNotEmpty()
{
  if(mybaseObjects->size() > 0) {
    execute();
  }
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theDirection, theSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theToSize, theFromSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theDirection,
                                                theToSize,
                                                theFromSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theDirection,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset));
}

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

#include "FeaturesAPI_Revolution.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
FeaturesAPI_Revolution::FeaturesAPI_Revolution(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Double& theToAngle,
  const ModelHighAPI_Double& theFromAngle,
  const std::string& theSelectionType)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    if (!theSelectionType.empty())
      mybaseObjects->setSelectionType(theSelectionType);
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theAxis, myaxis);
    setAngles(theToAngle, theFromAngle);
  }
}

//==================================================================================================
FeaturesAPI_Revolution::FeaturesAPI_Revolution(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Selection& theToObject,
  const ModelHighAPI_Double& theToOffset,
  const ModelHighAPI_Selection& theFromObject,
  const ModelHighAPI_Double& theFromOffset,
  const std::string& theSelectionType)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    if (!theSelectionType.empty())
      mybaseObjects->setSelectionType(theSelectionType);
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
void FeaturesAPI_Revolution::setNestedSketch(const ModelHighAPI_Reference& theSketch)
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
void FeaturesAPI_Revolution::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mysketch->setValue(ObjectPtr());
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAxis(const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theAxis, myaxis);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAngles(const ModelHighAPI_Double& theToAngle,
                                       const ModelHighAPI_Double& theFromAngle)
{
  fillAttribute(FeaturesPlugin_Revolution::CREATION_METHOD_BY_ANGLES(), mycreationMethod);
  fillAttribute(theToAngle, mytoAngle);
  fillAttribute(theFromAngle, myfromAngle);

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Revolution::setAngle(const ModelHighAPI_Double& theAngle)
{
  setAngles(theAngle, ModelHighAPI_Double());
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

  execIfBaseNotEmpty();
}

//==================================================================================================
void FeaturesAPI_Revolution::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrSketch = aBase->reference(FeaturesPlugin_Revolution::SKETCH_ID());
  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Revolution::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrAxis = aBase->selection(FeaturesPlugin_Revolution::AXIS_OBJECT_ID());

  theDumper << aBase << " = model.addRevolution(" << aDocName << ", ";
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
    AttributeDoublePtr anAttrToOffset = aBase->real(FeaturesPlugin_Revolution::TO_OFFSET_ID());
    AttributeSelectionPtr anAttrFromObject =
      aBase->selection(FeaturesPlugin_Revolution::FROM_OBJECT_ID());
    AttributeDoublePtr anAttrFromOffset = aBase->real(FeaturesPlugin_Revolution::FROM_OFFSET_ID());

    theDumper << ", " << anAttrToObject << ", " << anAttrToOffset <<
      ", " << anAttrFromObject << ", " << anAttrFromOffset;
  }

  // write explicitly the type of selection if it does not correspond
  // to the type of first selected shape
  if (!anAttrSketch->isInitialized()) {
    std::string aListSelType = anAttrObjects->selectionType();
    AttributeSelectionPtr aFirstSelection = anAttrObjects->value(0);
    GeomShapePtr aFirstShape = aFirstSelection->value();
    if (!aFirstShape)
      aFirstShape = aFirstSelection->context()->shape();
    if (!aFirstShape || aFirstShape->shapeType() != GeomAPI_Shape::shapeTypeByStr(aListSelType))
      theDumper << ", \"" << aListSelType << "\"";
  }

  theDumper << ")" << std::endl;

  if(anAttrSketch->isInitialized()) {
    theDumper << aBase << ".setNestedSketch(" << anAttrSketch << ")" << std::endl;
  }
}

//==================================================================================================
void FeaturesAPI_Revolution::execIfBaseNotEmpty()
{
  if(mybaseObjects->size() > 0) {
    execute();
  }
}

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
                            const ModelHighAPI_Double& theFromAngle,
                            const std::string& theSelectionType)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Revolution::ID());
  return RevolutionPtr(new FeaturesAPI_Revolution(aFeature,
                                                theBaseObjects,
                                                theAxis,
                                                theToAngle,
                                                theFromAngle,
                                                theSelectionType));
}

//==================================================================================================
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Selection& theToObject,
                            const ModelHighAPI_Double& theToOffset,
                            const ModelHighAPI_Selection& theFromObject,
                            const ModelHighAPI_Double& theFromOffset,
                            const std::string& theSelectionType)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Revolution::ID());
  return RevolutionPtr(new FeaturesAPI_Revolution(aFeature,
                                                theBaseObjects,
                                                theAxis,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset,
                                                theSelectionType));
}

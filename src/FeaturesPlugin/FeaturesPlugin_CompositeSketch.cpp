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

#include <FeaturesPlugin_CompositeSketch.h>
#include <FeaturesPlugin_Tools.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Revolution.h>

#include <GeomAPI_ShapeExplorer.h>


static const std::string COMPOSITESKETCH_VERSION_1("v9.6");

static void storeSubShape(const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                          ResultBodyPtr theResultBody,
                          const GeomShapePtr theShape,
                          const GeomAPI_Shape::ShapeType theType,
                          const std::string& theName);

//=================================================================================================
void FeaturesPlugin_CompositeSketch::initCompositeSketchAttribtues(const int theInitFlags)
{
  // Initialize sketch launcher.
  if(theInitFlags & InitSketchLauncher) {
    data()->addAttribute(SKETCH_ID(), ModelAPI_AttributeReference::typeId());
    ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SKETCH_ID());
  }

  // Initialize selection list.
  if(theInitFlags & InitBaseObjectsList) {
    AttributeSelectionListPtr anObjectsAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
        data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId()));
    myCurrentSelectionType = selectionList(BASE_OBJECTS_ID())->selectionType();
    anObjectsAttr->setWholeResultAllowed(true);
    if (!anObjectsAttr->isInitialized()) {
      // new feature, specify the version
      data()->setVersion(COMPOSITESKETCH_VERSION_1);
    }
  }
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::addFeature(std::string theID)
{
  FeaturePtr aNew = document()->addFeature(theID, false);
  if(aNew) {
    data()->reference(SKETCH_ID())->setValue(aNew);
  }

  // Set as current also after it becomes sub to set correctly enabled for other sketch subs.
  document()->setCurrentFeature(aNew, false);
  return aNew;
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::numberOfSubs(bool /*forTree*/) const
{
  ObjectPtr aObj = data()->reference(SKETCH_ID())->value();
  return aObj.get() ? 1 : 0;
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::subFeature(const int theIndex,
                                                                             bool /*forTree*/)
{
  FeaturePtr aSubFeature;
  if(theIndex == 0) {
    aSubFeature =
        std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
  }
  return aSubFeature;
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::subFeatureId(const int theIndex) const
{
  if(theIndex == 0) {
    FeaturePtr aFeature =
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
    if(aFeature.get()) {
      return aFeature->data()->featureId();
    }
  }

  return -1;
}

//=================================================================================================
bool FeaturesPlugin_CompositeSketch::isSub(ObjectPtr theObject) const
{
  bool isSubFeature = false;
  // Check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (aFeature.get()) {
    ObjectPtr aSub = data()->reference(SKETCH_ID())->value();
    isSubFeature = aSub == theObject;
  }
  return isSubFeature;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aBaseObjectsSelectionList.get() && aBaseObjectsSelectionList->size() > 0) {
    aBaseObjectsSelectionList->clear();
  }

  reference(SKETCH_ID())->setValue(ObjectPtr());
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::getBaseShapes(ListOfShape& theBaseShapesList,
                                                   const bool theIsMakeShells)
{
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  std::string anError;
  bool isOk = FeaturesPlugin_Tools::getShape(
      aBaseObjectsSelectionList, theIsMakeShells, theBaseShapesList, anError);
  if (!isOk)
    setError(anError);
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeResult(const GeomShapePtr theBaseShape,
                                        const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                        const int theIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShape, theMakeShape->shape());

  // Store generated edges/faces.
  storeGenerationHistory(aResultBody, theBaseShape, theMakeShape);

  setResult(aResultBody, theIndex);
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeGenerationHistory(ResultBodyPtr theResultBody,
                                        const GeomShapePtr theBaseShape,
                                        const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  GeomAPI_Shape::ShapeType aBaseShapeType = theBaseShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode = GeomAPI_Shape::SHAPE;

  switch(aBaseShapeType) {
    case GeomAPI_Shape::EDGE: {
            aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      break;
    }
    case GeomAPI_Shape::WIRE: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
      break;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
    }
    default: // [to avoid compilation warnings]
      break;
  }

  if(aShapeTypeToExplode == GeomAPI_Shape::VERTEX ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadGeneratedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX);
  }
  if(aShapeTypeToExplode == GeomAPI_Shape::EDGE ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadGeneratedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::EDGE);
  }
  std::list<std::shared_ptr<GeomAlgoAPI_MakeSweep> > aSweeps; // all sweeps collected
  std::shared_ptr<GeomAlgoAPI_MakeSweep> aMakeSweep =
    std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(theMakeShape);
  if(aMakeSweep.get()) {
    aSweeps.push_back(aMakeSweep);
  } else {
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeList =
      std::dynamic_pointer_cast<GeomAlgoAPI_MakeShapeList>(theMakeShape);
    if (aMakeList.get()) {
      ListOfMakeShape::const_iterator anIter = aMakeList->list().cbegin();
      for(; anIter != aMakeList->list().cend(); anIter++) {
        std::shared_ptr<GeomAlgoAPI_MakeSweep> aSweep =
          std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(*anIter);
        if (aSweep.get())
          aSweeps.push_back(aSweep);
      }
    }
  }
  std::list<std::shared_ptr<GeomAlgoAPI_MakeSweep> >::iterator aSweep = aSweeps.begin();
  for(; aSweep != aSweeps.end(); aSweep++) {
    // Store from shapes.
    storeShapes(theMakeShape, theResultBody, aBaseShapeType, (*aSweep)->fromShapes(), "From_");

    // Store to shapes.
    storeShapes(theMakeShape, theResultBody, aBaseShapeType, (*aSweep)->toShapes(), "To_");
  }
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeShapes(
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
  ResultBodyPtr theResultBody,
  const GeomAPI_Shape::ShapeType theBaseShapeType,
  const ListOfShape& theShapes,
  const std::string theName)
{
  GeomAPI_Shape::ShapeType aShapeTypeToExplore = GeomAPI_Shape::FACE;
  std::string aShapeTypeStr = "Face";
  switch(theBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplore = GeomAPI_Shape::VERTEX;
      aShapeTypeStr = "Vertex";
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      aShapeTypeToExplore = GeomAPI_Shape::EDGE;
      aShapeTypeStr = "Edge";
      break;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplore = GeomAPI_Shape::FACE;
      aShapeTypeStr = "Face";
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplore = GeomAPI_Shape::COMPOUND;
      break;
    }
    default: // [to avoid compilation warnings]
      break;
  }

  // Store shapes.
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;

    if(aShapeTypeToExplore == GeomAPI_Shape::COMPOUND) {
      std::string aName = theName + (aShape->shapeType() == GeomAPI_Shape::EDGE ? "Edge" : "Face");
      storeSubShape(theMakeShape, theResultBody, aShape, aShape->shapeType(), aName);
    } else {
      std::string aName = theName + aShapeTypeStr;
      storeSubShape(theMakeShape, theResultBody, aShape, aShapeTypeToExplore, aName);
      if (theBaseShapeType == GeomAPI_Shape::WIRE) { // issue 2289: special names also for vertices
        aName = theName + "Vertex";
        storeSubShape(theMakeShape, theResultBody, aShape, GeomAPI_Shape::VERTEX, aName);
      }
    }
  }
}

void storeSubShape(
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
  ResultBodyPtr theResultBody,
  const GeomShapePtr theShape,
  const GeomAPI_Shape::ShapeType theType,
  const std::string& theName)
{
  for(GeomAPI_ShapeExplorer anExp(theShape, theType); anExp.more(); anExp.next()) {
    GeomShapePtr aSubShape = anExp.current();
    if (!theResultBody->generated(aSubShape, theName)) {
      int aNbSubs = theResultBody->numberOfSubs();
      if (aNbSubs > 0) {
        // check the modified shape is in the result body, don't store it if not
        ListOfShape aNewShapes;
        theMakeShape->modified(aSubShape, aNewShapes);
        for (int i = 0; i < aNbSubs; ++i) {
          ResultBodyPtr aSubRes = theResultBody->subResult(i);
          GeomShapePtr aShape = aSubRes->shape();
          ListOfShape::iterator aNewIt = aNewShapes.begin();
          for (; aNewIt != aNewShapes.end(); ++aNewIt)
            if (aShape->isSubShape(*aNewIt, false))
              break;
          if (aNewIt != aNewShapes.end()) {
            // store from/to shapes as primitives and then store modification of them by the boolean
            aSubRes->generated(aSubShape, theName, false);
            aSubRes->loadModifiedShapes(theMakeShape, aSubShape, theType);
          }
        }
      }
      else {
        // store from/to shapes as primitives and then store modification of them by the boolean
        theResultBody->generated(aSubShape, theName, false);
        theResultBody->loadModifiedShapes(theMakeShape, aSubShape, theType);
      }
    }
  }
}

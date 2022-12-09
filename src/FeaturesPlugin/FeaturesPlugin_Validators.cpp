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

#include "FeaturesPlugin_Validators.h"

#include "FeaturesPlugin_Boolean.h"
#include "FeaturesPlugin_BooleanFuse.h"
#include "FeaturesPlugin_BooleanCommon.h"
#include "FeaturesPlugin_BooleanSmash.h"
#include "FeaturesPlugin_CompositeBoolean.h"
#include "FeaturesPlugin_Extrusion.h"
#include "FeaturesPlugin_Pipe.h"
#include "FeaturesPlugin_Union.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>

#include <GeomValidators_BodyShapes.h>
#include <GeomValidators_Face.h>
#include <GeomValidators_FeatureKind.h>
#include <GeomValidators_ShapeType.h>

#include <GeomAPI_DataMapOfShapeShape.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MapShapesAndAncestors.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

//==================================================================================================
bool FeaturesPlugin_ValidatorPipePath::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  AttributeSelectionPtr aPathAttrSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  if(!aPathAttrSelection.get()) {
// LCOV_EXCL_START
    theError = "Error: This validator can only work with path selector in \"Pipe\" feature.";
    return false;
// LCOV_EXCL_STOP
  }

  GeomShapePtr aPathShape = aPathAttrSelection->value();
  ResultPtr aContext = aPathAttrSelection->context();
  if(!aContext.get()) {
    theError = "Error: Empty context.";
    return false;
  }
  GeomShapePtr aContextShape = aContext->shape();
  if(aPathShape.get() && aPathShape->shapeType() == GeomAPI_Shape::WIRE &&
      !aPathShape->isEqual(aContextShape)) {
    theError = "Error: Local selection of wires not allowed.";
    return false;
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorPipeLocations::isValid(const AttributePtr& theAttribute,
                                                    const std::list<std::string>& theArguments,
                                                    Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!anAttrSelectionList.get()) {
// LCOV_EXCL_START
    theError =
      "Error: This validator can only work with selection list attributes in \"Pipe\" feature.";
    return false;
// LCOV_EXCL_STOP
  }
  std::shared_ptr<FeaturesPlugin_Pipe> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_Pipe>(theAttribute->owner());

  AttributeSelectionPtr aPathSelection = aFeature->selection(FeaturesPlugin_Pipe::PATH_OBJECT_ID());
  if (!aPathSelection.get()) {
// LCOV_EXCL_START
    theError = "Error: Path not selected.";
    return false;
// LCOV_EXCL_STOP
  }

  GeomShapePtr aPathShape = aPathSelection->value();
  if (!aPathShape.get()) {
    ResultPtr aContext = aPathSelection->context();
    if (!aContext.get()) {
      FeaturePtr aContFeat = aPathSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    aPathShape = aContext->shape();
  }

  if (!aPathShape.get()) {
    theError = "Error: Empty path shape.";
    return false;
  }

  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      FeaturePtr aContFeat = anAttrSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if (!aShape.get() && aContext.get()) {
      GeomShapePtr aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }

    if (!aPathShape->isSubShape(aShape)) {
      theError = "Error: Location should be a vertex subshape from path shape.";
      return false;
    }
  }

  return true;
}

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorPipeLocationsNumber::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  static const std::string aCreationMethodID = "creation_method";
  static const std::string aBaseObjectsID = "base_objects";
  static const std::string aLocationsID = "locations_objects";

  if(theFeature->getKind() != "Pipe") {
    theError = "Error: Feature \"%1\" does not supported by this validator.";
    theError.arg(theFeature->getKind());
    return false;
  }

  AttributeStringPtr aCreationMethodAttr = theFeature->string(aCreationMethodID);
  if(!aCreationMethodAttr.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aCreationMethodID);
    return false;
  }

  if(aCreationMethodAttr->value() != "locations") {
    return true;
  }

  AttributeSelectionListPtr aBaseObjectsSelectionList = theFeature->selectionList(aBaseObjectsID);
  if(!aBaseObjectsSelectionList.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseObjectsID);
    return false;
  }

  AttributeSelectionListPtr aLocationsSelectionList = theFeature->selectionList(aLocationsID);
  if(!aLocationsSelectionList.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseObjectsID);
    return false;
  }

  if(aLocationsSelectionList->size() > 0 &&
     aLocationsSelectionList->size() != aBaseObjectsSelectionList->size()) {
    theError = "Error: Number of locations should be the same as base objects.";
    return false;
  }

  return true;
}
// LCOV_EXCL_STOP

//==================================================================================================
bool FeaturesPlugin_ValidatorLoftSameTypeShape::isValid(
                                                const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  static const std::string aFirstObjetcID = "first_object";
  static const std::string aSecondObjetcID = "second_object";

  if (theFeature->getKind() != "Loft") {
    theError = "Error: Feature \"%1\" does not supported by this validator.";
    theError.arg(theFeature->getKind());
    return false;
  }

  AttributeSelectionPtr aFirstObjectsSelection = theFeature->selection(aFirstObjetcID);
  if ( !aFirstObjectsSelection->isInitialized()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aFirstObjetcID);
    return false;
  }

  AttributeSelectionPtr aSecondObjectsSelection = theFeature->selection(aSecondObjetcID);
  if (!aSecondObjectsSelection->isInitialized()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aSecondObjetcID);
    return false;
  }

  GeomShapePtr aFirstShape  = aFirstObjectsSelection->value();
  if (!aFirstShape.get()) {
    aFirstShape = aFirstObjectsSelection->context()->shape();
  }
  GeomShapePtr aSecondShape  = aSecondObjectsSelection->value();
  if (!aSecondShape.get()) {
    aSecondShape = aSecondObjectsSelection->context()->shape();
  }

  if (aFirstShape->isEqual(aSecondShape)) {
    theError = "Error: the shapes are equal";
    return false;
  }

  if (aFirstShape->shapeType()!=aSecondShape->shapeType()) {
    theError = "Error: the shapes have different type";
    return false;
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if(theArguments.empty()) {
    theError = "Error: Validator parameters is empty.";
    return false;
  }
// LCOV_EXCL_STOP

  // Checking attribute.
  if(!isValidAttribute(theAttribute, theArguments, theError)) {
    if(theError.empty()) {
      theError = "Error: Attribute contains unacceptable shape.";
    }
    return false;
  }

  GeomAPI_DataMapOfShapeShape aSelectedWiresFromObjects;
  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      AttributeSelectionPtr aSelectionAttr = aListAttr->value(anIndex);
      ResultPtr aContext = aSelectionAttr->context();
      if(!aContext.get() && !aSelectionAttr->contextFeature().get()) {
        theError = "Error: Empty context.";
        return false;
      }

      ResultConstructionPtr aResultConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
      if(!aResultConstruction.get()) {
        // It is not a result construction.
        // If shape is compound check that it contains only faces, edges or vertices.
        GeomShapePtr aShape = aSelectionAttr->value();
        if(!aShape.get()) {
          if (aContext.get()) {
            aShape = aContext->shape();
          } else {
            theError = "Error: Empty context.";
            return false;
          }
        }

        if(aShape->shapeType() == GeomAPI_Shape::COMPOUND) {
          for(GeomAPI_ShapeIterator anIt(aShape); anIt.more(); anIt.next()) {
            GeomShapePtr aSubShape = anIt.current();
            if (aSubShape->shapeType() > GeomAPI_Shape::VERTEX ||
                aSubShape->shapeType() < GeomAPI_Shape::FACE) {
              theError = "Error: Compound should contain only faces, edges or vertices.";
              return false;
            }
          }
        }

        continue;
      }

      GeomShapePtr aShape = aSelectionAttr->value();
      GeomShapePtr aContextShape = aResultConstruction->shape();
      if(!aShape.get()) {
        // Whole sketch selected.
        continue;
      } else {
        // Object from sketch selected.
        for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::WIRE); anExp.more(); anExp.next()) {
          GeomShapePtr aWire = anExp.current();
          if(aWire->orientation() != GeomAPI_Shape::FORWARD) {
            theError = "Error: Wire with wrong orientation selected.";
            return false;
          }

          if(aSelectedWiresFromObjects.isBound(aWire)) {
            theError =
              "Error: Objects with this wire already selected. Don't allow to select this object.";
            return false;
          }

          aSelectedWiresFromObjects.bind(aWire, aWire);
        }
      }
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorBaseForGenerationSketchOrSketchObjects::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  const std::string aBaseObjectsID = theArguments.front();

  AttributeSelectionListPtr aListAttr = theFeature->selectionList(aBaseObjectsID);
  if(!aListAttr.get()) {
// LCOV_EXCL_START
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseObjectsID);
    return false;
// LCOV_EXCL_STOP
  }

  std::set<ResultConstructionPtr> aSelectedSketches;
  std::set<ResultConstructionPtr> aSelectedSketchesFromObjects;

  for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
    AttributeSelectionPtr aSelectionAttr = aListAttr->value(anIndex);
    ResultPtr aContext = aSelectionAttr->context();
    if(!aContext.get()) {
      FeaturePtr aFeature = aSelectionAttr->contextFeature();
      if (!aFeature.get() || aFeature->results().empty()) {
        theError = "Error: Empty context.";
        return false;
      } else {
        aContext = aFeature->firstResult();
      }
    }

    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(!aResultConstruction.get()) {
      // It is not a result construction.
      continue;
    }

    GeomShapePtr aShape = aSelectionAttr->value();
    GeomShapePtr aContextShape = aResultConstruction->shape();
    if(!aShape.get()) {
      // Whole sketch selected.
      aSelectedSketches.insert(aResultConstruction);
    } else {
      // Object from sketch selected.
      aSelectedSketchesFromObjects.insert(aResultConstruction);
    }
  }


  for(std::set<ResultConstructionPtr>::const_iterator anIt = aSelectedSketches.cbegin();
      anIt != aSelectedSketches.cend();
      ++anIt) {
    ResultConstructionPtr aResultConstruction = *anIt;
    if(aSelectedSketchesFromObjects.find(aResultConstruction) !=
        aSelectedSketchesFromObjects.cend()) {
      theError = "Sketch and objects from it can not be selected at the same time.";
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValidAttribute(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
  if(!theAttribute.get()) {
// LCOV_EXCL_START
    theError = "Error: Empty attribute.";
    return false;
// LCOV_EXCL_STOP
  }

  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

    const std::string& aVersion = theAttribute->owner()->data()->version();
    std::string aSelType;
    if (!aVersion.empty())
      aSelType = aListAttr->selectionType();

    std::list<std::string> anApplicableTypes;
    switch (GeomValidators_ShapeType::shapeType(aSelType)) {
    case GeomValidators_ShapeType::Vertex:
      anApplicableTypes.push_back("vertex");
      break;
    case GeomValidators_ShapeType::Edge:
      anApplicableTypes.push_back("edge");
      anApplicableTypes.push_back("wire");
      break;
    case GeomValidators_ShapeType::Face:
      anApplicableTypes.push_back("face");
      anApplicableTypes.push_back("shell");
      // wire should not be the first in this list to be able to check
      // the type of selection when evaluating shape by shape
      anApplicableTypes.push_back("wire");
      break;
    default:
      anApplicableTypes = theArguments;
      break;
    }

    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      // If at least one attribute is invalid, the result is false.
      if(!isValidAttribute(aListAttr->value(anIndex), anApplicableTypes, theError)) {
        return false;
      }
    }
  } else if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    // Getting context.
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    if(!aContext.get() && !anAttr->contextFeature().get()) {
      theError = "Error: Attribute have empty context.";
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    GeomShapePtr aContextShape;
    if(!aShape.get() && aContext.get()) {
      aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Error: Empty shape selected";
      return false;
    }

    ResultConstructionPtr aConstruction;
    if (!aContext.get() && anAttr->contextFeature()->results().size() == 1) {
      aContext = anAttr->contextFeature()->firstResult();
    }
    if (aContext.get())
      aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      // Construction selected. Check that it is not infinite.
      if(aConstruction->isInfinite() && !aConstruction->shape()->isVertex()) {
        theError = "Error: Infinite constructions is not allowed as base.";
        return false;
      }

      aContextShape = aContext->shape();
      if(aShape->isEqual(aContextShape)) {
        // Whole construction selected. Check that it has faces.
        if((theArguments.front() == "face" && aConstruction->facesNum() > 0) ||
            theArguments.front() == "edge") {
          return true;
        }
      } else {
        // CUT operation supports only FACE or WIRE as a tool base
        std::shared_ptr<FeaturesPlugin_CompositeBoolean> aComposite =
            std::dynamic_pointer_cast<FeaturesPlugin_CompositeBoolean>(
            ModelAPI_Feature::feature(theAttribute->owner()));
        if (aComposite &&
            aComposite->operationType() == FeaturesPlugin_CompositeBoolean::BOOL_CUT) {
          return aShape->shapeType() == GeomAPI_Shape::WIRE ||
                 aShape->shapeType() == GeomAPI_Shape::FACE;
        }
      }
    }

    if(!aConstruction && aContextShape.get() && !aShape->isEqual(aContextShape)) {
      // Local selection on body does not allowed.
      theError =
        "Error: Selected shape is in the local selection. Only global selection is allowed.";
      return false;
    }

    // Check that object is a shape with allowed type.
    GeomValidators_ShapeType aShapeTypeValidator;
    if(!aShapeTypeValidator.isValid(anAttr, theArguments, theError)) {
      theError = "Error: Selected shape has unacceptable type. Acceptable types are: faces or "
                 "wires on sketch, whole sketch (if it has at least one face), "
                 "and whole objects with shape types: %1";
      std::string anArgumentString;
      for(auto anIt = theArguments.cbegin(); anIt != theArguments.cend(); ++anIt) {
        if (!anArgumentString.empty())
          anArgumentString += ", ";
        anArgumentString += *anIt;
      }
      theError.arg(anArgumentString);
      return false;
    }

  } else {
// LCOV_EXCL_START
    theError = "Error: Attribute \"%1\" does not supported by this validator.";
    theError.arg(anAttributeType);
    return false;
// LCOV_EXCL_STOP
  }

  return true;
}

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorCompositeLauncher::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeReference::typeId()) {
    theError = "Error: The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }
  if (theArguments.size() != 2) {
    theError = "Error: Wrong parameters in XML definition for %1 type";
    theError.arg(theAttribute->attributeType());
    return false;
  }
  // first argument is for the base attribute, second - for skipping feature kind
  std::list<std::string>::const_iterator anIt = theArguments.begin();
  std::string aBaseAttributeId = *anIt;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  AttributePtr aBaseAttribute = aFeature->attribute(aBaseAttributeId);
  if (!aBaseAttribute.get()) {
    theError = "Wrong parameters in XML definition for %1 type";
    theError.arg(theAttribute->attributeType());
    return false;
  }
  if (aBaseAttribute->isInitialized()) // when base list of composite feature is already filled,
    // this validator is not necessary anymore
    return true;

  anIt++;
  std::string aFeatureAttributeKind = *anIt;
  GeomValidators_FeatureKind* aValidator = new GeomValidators_FeatureKind();
  // check whether the selection is on the sketch
  std::list<std::string> anArguments;
  anArguments.push_back(aFeatureAttributeKind);

  bool aFeatureKind = aValidator->isValid(theAttribute, theArguments, theError);
  bool aPlanarFace = false;
  // check if selection has Face selected
  GeomValidators_ShapeType* aShapeType = new GeomValidators_ShapeType();
  anArguments.clear();
  anArguments.push_back("face");
  aPlanarFace = aShapeType->isValid(theAttribute, anArguments, theError);

  bool aValid = !aFeatureKind && aPlanarFace;
  return aValid;
}
// LCOV_EXCL_STOP

//==================================================================================================
bool FeaturesPlugin_ValidatorExtrusionDir::isValid(
                                               const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  if(theArguments.size() != 2) {
// LCOV_EXCL_START
    theError = "Error: Validator should be used with 2 parameters for extrusion.";
    return false;
// LCOV_EXCL_STOP
  }

  std::list<std::string>::const_iterator anArgsIt = theArguments.begin();

  AttributePtr aCheckAttribute = theFeature->attribute(*anArgsIt);
  ++anArgsIt;

  GeomShapePtr aDirShape;
  AttributeSelectionPtr aSelAttr = theFeature->selection(*anArgsIt);
  if(aSelAttr.get()) {
    aDirShape = aSelAttr->value();
    if(!aDirShape.get()) {
      ResultPtr aContext = aSelAttr->context();
      if(!aContext.get()) {
        FeaturePtr aFeature = aSelAttr->contextFeature();
        if (aFeature.get() && !aFeature->results().empty()) {
          aContext = aFeature->firstResult();
        }
      }

      if(aContext.get()) {
        aDirShape = aContext->shape();
      }

      if (aDirShape.get() && aDirShape->isCompound()) {
        GeomAPI_ShapeIterator anIt(aDirShape);
        aDirShape = anIt.current();
      }
    }
  }

  if(!aDirShape.get() || aDirShape->isNull() ||
     aDirShape->shapeType() != GeomAPI_Shape::EDGE) {
    // Check that dir can be empty.
    if(!isShapesCanBeEmpty(aCheckAttribute, theError)) {
      theError = "Error: Base objects list contains vertex or edge, so attribute \"%1\" "
                 "can not be used with default value. Select direction for extrusion.";
      theError.arg(*anArgsIt);
      return false;
    } else {
      return true;
    }
  }

  std::shared_ptr<GeomAPI_Edge> aDirEdge(new GeomAPI_Edge(aDirShape));

  // If faces selected check that direction not parallel with them.
  AttributeSelectionListPtr aListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aCheckAttribute);
  for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
    AttributeSelectionPtr anAttr = aListAttr->value(anIndex);
    GeomShapePtr aShapeInList = anAttr->value();
    if(!aShapeInList.get()) {
      aShapeInList = anAttr->context()->shape();
    }
    bool isParallel = true;
    if(aShapeInList->shapeType() == GeomAPI_Shape::FACE ||
       aShapeInList->shapeType() == GeomAPI_Shape::SHELL) {
      for(GeomAPI_ShapeExplorer
          anExp(aShapeInList, GeomAPI_Shape::FACE); anExp.more(); anExp.next()) {
        std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(anExp.current()));
        isParallel = GeomAlgoAPI_ShapeTools::isParallel(aDirEdge, aFace);
        if(isParallel) {
          break;
        }
      }
    } else if(aShapeInList->shapeType() == GeomAPI_Shape::COMPOUND) {
      std::shared_ptr<GeomAPI_PlanarEdges> aPlanarEdges =
        std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aShapeInList);
      if(aPlanarEdges.get()) {
        std::shared_ptr<GeomAPI_Dir> aSketchDir = aPlanarEdges->norm();
        if(aDirEdge->isLine()) {
          std::shared_ptr<GeomAPI_Dir> aDir = aDirEdge->line()->direction();
          isParallel = fabs(aSketchDir->angle(aDir) - M_PI / 2.0) < 10e-7;
        } else {
          isParallel = false;
        }
      } else {
        isParallel = false;
      }
    } else {
      isParallel = false;
    }
    if(isParallel) {
      theError =
        "Error: Direction is parallel to one of the selected face or face on selected shell.";
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorExtrusionDir::isShapesCanBeEmpty(const AttributePtr& theAttribute,
                                                              Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if(!theAttribute.get()) {
    return true;
  }
// LCOV_EXCL_STOP

  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      // If at least one attribute is invalid, the result is false.
      if(!isShapesCanBeEmpty(aListAttr->value(anIndex), theError)) {
        return false;
      }
    }
  } else if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    // Getting context.
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    if(!aContext.get() && !anAttr->contextFeature().get()) {
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    if(!aShape.get() && aContext.get()) {
      GeomShapePtr aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      return false;
    }

    if(aShape->shapeType() == GeomAPI_Shape::VERTEX ||
       aShape->shapeType() == GeomAPI_Shape::EDGE ||
       !aShape->isPlanar()) {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorExtrusionBoundaryFace::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  // Collect all necessary attributes and try to build prism

  // base face
  AttributeSelectionListPtr aBaseShapeAttr =
      aFeature->selectionList(FeaturesPlugin_Extrusion::BASE_OBJECTS_ID());
  ListOfShape aBaseShapeList;
  std::string anError;
  if (!FeaturesPlugin_Tools::getShape(aBaseShapeAttr, true, aBaseShapeList, anError)) {
    theError = anError;
    return false;
  }

  // direction
  AttributeSelectionPtr aSelection =
      aFeature->selection(FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID());
  GeomShapePtr aShape = aSelection->value();
  if (!aShape.get() && aSelection->context().get())
    aShape = aSelection->context()->shape();

  GeomEdgePtr anEdge;
  if (aShape.get()) {
    if (aShape->isEdge())
      anEdge = aShape->edge();
    else if (aShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aShape);
      anEdge = anIt.current()->edge();
    }
  }

  std::shared_ptr<GeomAPI_Dir> aDir;
  if (anEdge.get() && anEdge->isLine())
    aDir = anEdge->line()->direction();

  // from/to shapes
  GeomShapePtr aFromShape, aToShape;
  aSelection = aFeature->selection(FeaturesPlugin_Extrusion::TO_OBJECT_ID());
  if (aSelection.get()) {
    aToShape = aSelection->value();
    if (!aToShape.get() && aSelection->context().get())
      aToShape = aSelection->context()->shape();
    if (aToShape.get() && aToShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aToShape);
      aToShape = anIt.current();
    }
    if (aToShape.get() && !aToShape->isFace()) {
      theError = "\"To\" shape is not a face";
      return false;
    }
  }
  aSelection = aFeature->selection(FeaturesPlugin_Extrusion::FROM_OBJECT_ID());
  if (aSelection.get()) {
    aFromShape = aSelection->value();
    if (!aFromShape.get() && aSelection->context().get())
      aFromShape = aSelection->context()->shape();
    if (aFromShape.get() && aFromShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aFromShape);
      aFromShape = anIt.current();
    }
    if (aFromShape.get() && !aFromShape->isFace()) {
      theError = "\"From\" shape is not a face";
      return false;
    }
  }

  double aToSize = aFeature->real(FeaturesPlugin_Extrusion::TO_OFFSET_ID())->value();
  double aFromSize = aFeature->real(FeaturesPlugin_Extrusion::FROM_OFFSET_ID())->value();

  // check extrusion
  for (ListOfShape::iterator anIt = aBaseShapeList.begin(); anIt != aBaseShapeList.end(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIt;

    std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo(
        new GeomAlgoAPI_Prism(aBaseShape, aDir, aToShape, aToSize, aFromShape, aFromSize));
    bool isFailed = GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPrismAlgo,
                                                                    FeaturesPlugin_Extrusion::ID(),
                                                                    anError);
    if (isFailed) {
      theError = anError;
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorBooleanSelection::isValid(const AttributePtr& theAttribute,
                                                       const std::list<std::string>& theArguments,
                                                       Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!anAttrSelectionList.get()) {
// LCOV_EXCL_START
    theError =
      "Error: This validator can only work with selection list attributes in \"Boolean\" feature.";
    return false;
// LCOV_EXCL_STOP
  }
  std::shared_ptr<FeaturesPlugin_Boolean> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_Boolean>(theAttribute->owner());
  FeaturesPlugin_Boolean::OperationType anOperationType = aFeature->operationType();

  for(int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if(!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      FeaturePtr aContFeat = anAttrSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aResultConstruction.get()) {
      if (anOperationType != FeaturesPlugin_Boolean::BOOL_FILL
          || theAttribute->id() != FeaturesPlugin_Boolean::TOOL_LIST_ID()) {
        theError = "Error: Result construction not allowed for selection.";
        return false;
      }
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if(!aShape.get() && aContext.get()) {
      GeomShapePtr aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }
    if (aContext.get() && !aShape->isEqual(aContext->shape())) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    GeomAPI_Shape::ShapeType aShapeType = aShape->shapeType();
    std::set<GeomAPI_Shape::ShapeType> anAllowedTypes;
    if(anOperationType == FeaturesPlugin_Boolean::BOOL_FUSE) {
      anAllowedTypes.insert(GeomAPI_Shape::EDGE);
      anAllowedTypes.insert(GeomAPI_Shape::FACE);
      anAllowedTypes.insert(GeomAPI_Shape::SOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPSOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPOUND);
    } else if (anOperationType == FeaturesPlugin_Boolean::BOOL_FILL
               || anOperationType == FeaturesPlugin_Boolean::BOOL_CUT)
    {
      anAllowedTypes.insert(GeomAPI_Shape::VERTEX);
      anAllowedTypes.insert(GeomAPI_Shape::EDGE);
      anAllowedTypes.insert(GeomAPI_Shape::WIRE);
      anAllowedTypes.insert(GeomAPI_Shape::FACE);
      anAllowedTypes.insert(GeomAPI_Shape::SHELL);
      anAllowedTypes.insert(GeomAPI_Shape::SOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPSOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPOUND);
    } else {
      anAllowedTypes.insert(GeomAPI_Shape::SOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPSOLID);
      anAllowedTypes.insert(GeomAPI_Shape::COMPOUND);
    }

    if(anAllowedTypes.find(aShapeType) == anAllowedTypes.end()
      || (aResultConstruction.get() && aShapeType != GeomAPI_Shape::FACE)) {
      theError = "Error: Selected shape has the wrong type.";
      return false;
    }

  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorFilletSelection::isValid(const AttributePtr& theAttribute,
                                                      const std::list<std::string>& theArguments,
                                                      Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!anAttrSelectionList.get()) {
// LCOV_EXCL_START
    theError =
      "Error: This validator can only work with selection list attributes in \"Fillet\" feature.";
    return false;
// LCOV_EXCL_STOP
  }

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  // Check all selected entities are sub-shapes of single solid
  GeomShapePtr aBaseSolid;
  for(int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if(!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      FeaturePtr aContFeat = anAttrSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size() ||
          aContFeat->firstResult()->groupName() != ModelAPI_ResultBody::group()) {
        theError = "Error: Empty selection context.";
        return false;
      }
      if (aContFeat->results().size() == 1)
        aContext = aContFeat->firstResult();
      else {
        theError = "Error: Too many shapes selected.";
        return false;
      }
    }

    ResultBodyPtr aContextOwner = ModelAPI_Tools::bodyOwner(aContext, true);
    GeomShapePtr anOwner = aContext->shape();
    GeomShapePtr aTopLevelOwner = aContextOwner.get() ? aContextOwner->shape() : anOwner;

    if (!anOwner) {
      theError = "Error: wrong feature is selected.";
      return false;
    }

    if (anOwner->shapeType() != GeomAPI_Shape::SOLID &&
        anOwner->shapeType() != GeomAPI_Shape::COMPSOLID) {
      theError = "Error: Not all selected shapes are sub-shapes of solids.";
      return false;
    }

    if (!aBaseSolid)
      aBaseSolid = aTopLevelOwner;
    else if (!aBaseSolid->isEqual(aTopLevelOwner)) {
      theError = "Error: Sub-shapes of different solids have been selected.";
      return false;
    }
  }

  return true;
}


//==================================================================================================
bool FeaturesPlugin_ValidatorFillet1DSelection::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    // LCOV_EXCL_START
    theError =
      "Error: This validator can only work with selection list attributes in \"Fillet\" feature.";
    return false;
    // LCOV_EXCL_STOP
  }

  // Validate wires selection
  if (!theArguments.empty() &&
      theArguments.front() == "wire") {
    for (int ind = 0; ind < anAttrSelectionList->size(); ++ind) {
      AttributeSelectionPtr aCurSel = anAttrSelectionList->value(ind);
      //GeomShapePtr aContext = aCurSel->context()->shape();
      GeomShapePtr aWire = aCurSel->value();
      if (aWire->shapeType() != GeomAPI_Shape::WIRE) {
        theError = "Selected shape is not a wire";
        return false;
      }
    }
    return true;
  }

  // check each selected vertex is a sharp corner between adjacent edges,
  // and these edges are in the same plane
  std::map<GeomShapePtr, MapShapeToShapes> aWireSubshapes;
  int aNbSel = anAttrSelectionList->size();
  for (int ind = 0; ind < aNbSel; ++ind) {
    AttributeSelectionPtr aCurSel = anAttrSelectionList->value(ind);
    GeomShapePtr aContext = aCurSel->context()->shape();
    GeomShapePtr aVertex = aCurSel->value();
    // check wire already processed, if not, store all vertices and edges, sharing them
    std::map<GeomShapePtr, MapShapeToShapes>::iterator aProcessed = aWireSubshapes.find(aContext);
    if (aProcessed == aWireSubshapes.end()) {
      if (aContext->shapeType() != GeomAPI_Shape::WIRE) {
        theError = "Selected vertex is not a wire corner";
        return false;
      }
      if (aVertex->shapeType() != GeomAPI_Shape::VERTEX) {
        theError = "Selected shape is not a vertex";
        return false;
      }

      GeomAlgoAPI_MapShapesAndAncestors aMapVE(aContext, GeomAPI_Shape::VERTEX,
                                                         GeomAPI_Shape::EDGE);
      aWireSubshapes[aContext] = aMapVE.map();
      aProcessed = aWireSubshapes.find(aContext);
    }

    // check the vertex
    MapShapeToShapes::iterator aFound = aProcessed->second.find(aVertex);
    if (aFound == aProcessed->second.end()) {
      theError = "Selected vertex does not exist in the wire";
      return true;
    }
    else if (aFound->second.size() != 2) {
      theError = "Vertex should be shared between 2 edges exactly";
      return false;
    }

    ListOfShape anEdges;
    anEdges.insert(anEdges.end(), aFound->second.begin(), aFound->second.end());
    GeomPlanePtr aPlane = GeomAlgoAPI_ShapeTools::findPlane(anEdges);
    if (!aPlane) {
      theError = "Error: Edges are not planar";
      return false;
    }

    GeomEdgePtr anEdge1(new GeomAPI_Edge(anEdges.front()));
    GeomEdgePtr anEdge2(new GeomAPI_Edge(anEdges.back()));
    GeomVertexPtr aSharedVertex(new GeomAPI_Vertex(aVertex));
    if (GeomAlgoAPI_ShapeTools::isTangent(anEdge1, anEdge2, aSharedVertex)) {
      theError = "Error: Edges are tangent";
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorPartitionSelection::isValid(const AttributePtr& theAttribute,
                                                       const std::list<std::string>& theArguments,
                                                       Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!anAttrSelectionList.get()) {
// LCOV_EXCL_START
    theError = "Error: This validator can only work with selection list in \"Partition\" feature.";
    return false;
// LCOV_EXCL_STOP
  }

  for(int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelectAttr = anAttrSelectionList->value(anIndex);

    //GeomValidators_BodyShapes aBodyValidator;
    //if(aBodyValidator.isValid(aSelectAttr, theArguments, theError)) {
    //  continue;
    //}

    GeomValidators_FeatureKind aFeatureKindValidator;
    if(aFeatureKindValidator.isValid(aSelectAttr, theArguments, theError)) {
      continue;
    }

    ResultPtr aContext = aSelectAttr->context();
    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aResultConstruction.get()) {
      theError = "Error: Only body shapes and construction planes are allowed for selection.";
      return false;
    }

    ResultBodyPtr aResultBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContext);
    if(aResultBody.get()) {
      continue;
    }
    FeaturePtr aResultFeature = aSelectAttr->contextFeature();
    if(aResultFeature.get()) {
      bool aOkRes = false;
      std::list<ResultPtr>::const_iterator aFRes = aResultFeature->results().cbegin();
      for(; aFRes != aResultFeature->results().cend() && !aOkRes; aFRes++) {
        ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aFRes);
        if (aBody.get() && !aBody->isDisabled())
          aOkRes = true;
      }
      if (aOkRes)
        continue;
    }

    theError = "Error: Only body shapes and construction planes are allowed for selection.";
    return false;
  }

  theError = "";
  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorRemoveSubShapesSelection::isValid(const AttributePtr& theAttribute,
                                                     const std::list<std::string>& theArguments,
                                                     Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aSubShapesAttrList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aSubShapesAttrList.get()) {
// LCOV_EXCL_START
    theError =
      "Error: This validator can only work with selection list in \"Remove Sub-Shapes\" feature.";
    return false;
// LCOV_EXCL_STOP
  }

  static const std::string aBaseShapeID = "base_shape";
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionPtr aShapeAttrSelection = aFeature->selection(aBaseShapeID);

  if(!aShapeAttrSelection.get()) {
// LCOV_EXCL_START
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseShapeID);
    return false;
// LCOV_EXCL_STOP
  }

  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  ResultPtr aContext = aShapeAttrSelection->context();
  if(!aContext.get()) {
    theError = "Error: Empty context.";
    return false;
  }
  if(!aBaseShape.get()) {
    aBaseShape = aContext->shape();
  }
  if(!aBaseShape.get()) {
    theError = "Error: Empty base shape.";
    return false;
  }

  std::list<GeomShapePtr> aSubShapes = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aBaseShape);
  for(int anIndex = 0; anIndex < aSubShapesAttrList->size(); ++anIndex) {
    bool isSameFound = false;
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
    GeomShapePtr aShapeToAdd = anAttrSelectionInList->value();
    for (ListOfShape::const_iterator anIt = aSubShapes.cbegin(); anIt != aSubShapes.cend(); ++anIt)
    {
      if ((*anIt)->isEqual(aShapeToAdd)) {
        isSameFound = true;
        break;
      }
    }
    if (!isSameFound) {
      theError = "Error: Only sub-shapes of selected shape is allowed for selection.";
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorRemoveSubShapesResult::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  static const std::string aBaseShapeID = "base_shape";
  static const std::string aSubShapesID = "subshapes_to_keep";

// LCOV_EXCL_START
  if(theFeature->getKind() != "Remove_SubShapes") {
    theError = "Error: Feature \"%1\" does not supported by this validator.";
    theError.arg(theFeature->getKind());
    return false;
  }

  AttributeSelectionPtr aShapeAttrSelection = theFeature->selection(aBaseShapeID);
  if(!aShapeAttrSelection.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseShapeID);
    return false;
  }

  AttributeSelectionListPtr aSubShapesAttrList = theFeature->selectionList(aSubShapesID);
  if(!aSubShapesAttrList.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aSubShapesID);
    return false;
  }
// LCOV_EXCL_STOP

  // Copy base shape.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if(!aBaseShape.get()) {
    theError = "Error: Base shape is empty.";
    return false;
  }
  GeomShapePtr aResultShape = aBaseShape->emptyCopied();

  if (aSubShapesAttrList->size() == 0) {
    theError = "Error: Resulting shape is not valid.";
    return false;
  }

  // Copy sub-shapes from list to new shape.
  for(int anIndex = 0; anIndex < aSubShapesAttrList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
    GeomShapePtr aShapeToAdd = anAttrSelectionInList->value();
    GeomAlgoAPI_ShapeBuilder::add(aResultShape, aShapeToAdd);
  }

  // Check new shape.
  if(!GeomAlgoAPI_ShapeTools::isShapeValid(aResultShape)) {
    theError = "Error: Resulting shape is not valid.";
    return false;
  }

  return true;
}

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorUnionSelection::isValid(const AttributePtr& theAttribute,
                                                     const std::list<std::string>& theArguments,
                                                     Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aBaseObjectsAttrList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aBaseObjectsAttrList.get()) {
    theError = "Error: This validator can only work with selection list in \"%1\" feature.";
    theError.arg(FeaturesPlugin_Union::ID());
    return false;
  }

  for(int anIndex = 0; anIndex < aBaseObjectsAttrList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelectionInList = aBaseObjectsAttrList->value(anIndex);
    ResultPtr aContext = anAttrSelectionInList->context();
    if (!aContext.get()) {
      theError = "Error: selection is invalid.";
      return false;
    }

    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      theError = "Error: Result construction not allowed for selection.";
      return false;
    }

    GeomShapePtr aShape = anAttrSelectionInList->value();
    GeomShapePtr aContextShape = aContext->shape();
    if (aShape.get() && aContextShape.get() && !aContextShape->isEqual(aShape)) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    ResultBodyPtr aResult =
      std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContext);
    if(!aResult.get()) {
      continue;
    }

    if(aResult->numberOfSubs() > 0) {
      theError = "Error: Whole compsolids not allowed for selection.";
      return false;
    }
  }

  return true;
}
// LCOV_EXCL_STOP

//==================================================================================================
bool FeaturesPlugin_ValidatorUnionArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  // Check feature kind.
  if(theFeature->getKind() != FeaturesPlugin_Union::ID()) {
    theError = "Error: This validator supports only \"%1\" feature.";
    theError.arg(FeaturesPlugin_Union::ID());
    return false;
  }

  // Get base objects attribute list.
  AttributeSelectionListPtr aBaseObejctsAttrList =
    theFeature->selectionList(FeaturesPlugin_Union::BASE_OBJECTS_ID());
  if(!aBaseObejctsAttrList.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(FeaturesPlugin_Union::BASE_OBJECTS_ID());
    return false;
  }
// LCOV_EXCL_STOP

  // Get all shapes.
  GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::COMPSOLID;
  ListOfShape aBaseShapesList;
  for(int anIndex = 0; anIndex < aBaseObejctsAttrList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelectionInList = aBaseObejctsAttrList->value(anIndex);
    GeomShapePtr aShape = anAttrSelectionInList->value();
    if (!aShape.get()) {
      continue;
    }
    aBaseShapesList.push_back(aShape);
    aType = aShape->shapeType() == GeomAPI_Shape::FACE ? GeomAPI_Shape::SHELL :
                                                         GeomAPI_Shape::COMPSOLID;
  }

  // Make compound and find connected.
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aBaseShapesList);
  ListOfShape aResults;
  GeomAlgoAPI_ShapeTools::combineShapes(aCompound, aType, aResults);

  if(aResults.size() > 1 || (aResults.size() == 1 && aResults.front()->shapeType() > aType)) {
    theError = "Error: Not all shapes have shared topology.";
    return false;
  }

  return true;
}

bool FeaturesPlugin_ValidatorConcealedResult::isValid(const AttributePtr& theAttribute,
                                            const std::list<std::string>& theArguments,
                                            Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeReference::typeId()) {
// LCOV_EXCL_START
    theError = "Error: The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
// LCOV_EXCL_STOP
  }

  AttributeReferencePtr aRefAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeReference>
                                                                               (theAttribute);
  ObjectPtr aRefObject = aRefAttribute->value();
  if (!aRefObject.get()) {
    theError = "Error: Empty feature.";
    return false;
  }

  FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aRefObject);
  if (!aRefFeature.get()) {
    theError = "Error: Empty feature.";
    return false;
  }
  std::list<std::shared_ptr<ModelAPI_Result> > aResults;
  ModelAPI_Tools::getConcealedResults(aRefFeature, aResults);

  size_t aConcealedResults = aResults.size();
  if (!aConcealedResults && !theArguments.empty()) {
    // find if these results are touched by the feature in another attribute
    std::list<std::string>::const_iterator anIt = theArguments.begin();
    std::string aRecoveredList = *anIt;
    if (!aRecoveredList.empty()) {
      std::shared_ptr<ModelAPI_AttributeRefList> aParameterList =
                                 theAttribute->owner()->data()->reflist(aRecoveredList);
      if (aParameterList.get())
        aConcealedResults = aParameterList->size();
    }
  }

  if (aConcealedResults == 0)
    theError = "Error: No concealed results.";

  return theError.empty();
}

bool FeaturesPlugin_ValidatorCircular::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  static std::list<std::string> aEdgeArg(1, "circle");
  static std::list<std::string> aFaceArg(1, "cylinder");

  Events_InfoMessage aError;
  bool isValid = GeomValidators_ShapeType().isValid(theAttribute, aEdgeArg, aError);
  if (!isValid) {
    isValid = GeomValidators_Face().isValid(theAttribute, aFaceArg, aError);
    if (!isValid)
      theError = "The shape neither circle nor cylinder";
  }
  return isValid;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBooleanArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if (theArguments.size() != 2)
  {
    theError = "Wrong number of arguments (expected 2).";
    return false;
  }
// LCOV_EXCL_STOP

  int anObjectsToolsNb[2] = { 0,  0 };

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  bool isAllInSameCompSolid = true;
  ResultBodyPtr aCompSolid;

  for (int* anArgNbIt = anObjectsToolsNb; anIt != aLast; ++anIt, ++anArgNbIt) {
    AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
    if (anAttrSelList)
    {
      *anArgNbIt = anAttrSelList->size();
      if (isAllInSameCompSolid) {
        for (int anIndex = 0; anIndex < *anArgNbIt; ++anIndex)
        {
          AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
          ResultPtr aContext = anAttr->context();
          ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
          if (aResCompSolidPtr.get())
          {
            if (aCompSolid.get())
            {
              isAllInSameCompSolid = aCompSolid == aResCompSolidPtr;
            }
            else
            {
              aCompSolid = aResCompSolidPtr;
            }
          }
          else
          {
            isAllInSameCompSolid = false;
            break;
          }
        }
      }
    }
  }

  std::shared_ptr<FeaturesPlugin_Boolean> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_Boolean>(theFeature);
  FeaturesPlugin_Boolean::OperationType anOperationType = aFeature->operationType();

  if (anOperationType == FeaturesPlugin_Boolean::BOOL_FUSE)
  {
    // Fuse operation
    if (anObjectsToolsNb[0] + anObjectsToolsNb[1] < 2)
    {
      theError = "Not enough arguments for Fuse operation.";
      return false;
    }
    else if (isAllInSameCompSolid)
    {
      theError = "Operations only between sub-shapes of the same shape not allowed.";
      return false;
    }
  }
  else
  {
    if (anObjectsToolsNb[0] < 1) // check number of objects
    {
      theError = "Objects not selected.";
      return false;
    }
    if (anObjectsToolsNb[1] < 1) // check number of tools
    {
      theError = "Tools not selected.";
      return false;
    }
    if (isAllInSameCompSolid)
    {
      theError = "Operations only between sub-shapes of the same shape not allowed.";
      return false;
    }
  }

  return true;
}

//=================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorBooleanArguments::isNotObligatory(std::string theFeature,
                                                               std::string theAttribute)
{
  if (theAttribute == "main_objects" || theAttribute == "tool_objects")
  {
    return true;
  }

  return false;
}
// LCOV_EXCL_STOP

//==================================================================================================
bool FeaturesPlugin_ValidatorBooleanSmashSelection::isValid(
  const AttributePtr& theAttribute,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  std::shared_ptr<FeaturesPlugin_BooleanSmash> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_BooleanSmash>(theAttribute->owner());

  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!aFeature.get() || !anAttrSelectionList.get()) {
// LCOV_EXCL_START
    theError =
      "Error: Validator used in wrong feature or attribute";
    return false;
// LCOV_EXCL_STOP
  }

  AttributeSelectionListPtr anOtherAttrSelectionList;
  if (theAttribute->id() == FeaturesPlugin_BooleanSmash::OBJECT_LIST_ID()) {
    anOtherAttrSelectionList =
      aFeature->selectionList(FeaturesPlugin_BooleanSmash::TOOL_LIST_ID());
  } else {
    anOtherAttrSelectionList =
      aFeature->selectionList(FeaturesPlugin_BooleanSmash::OBJECT_LIST_ID());
  }

  GeomAPI_Shape::ShapeType aSelectedShapesType = GeomAPI_Shape::SHAPE;
  GeomAPI_DataMapOfShapeShape aSelectedCompSolidsInOtherList;
  GeomPlanePtr aFacesPln;

  for (int anIndex = 0; anIndex < anOtherAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anOtherAttrSelectionList->value(anIndex);

    if (anAttrSelection->contextFeature().get()) {
      theError = "Error: Features not allowed for selection.";
      return false;
    }

    ResultPtr aContext = anAttrSelection->context();
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if (!aShape.get()) {
      if (!aContext.get()) {
        theError = "Error: Empty selection.";
        return false;
      }
      aShape = aContext->shape();
    }

    if (aShape->isSolid() || aShape->isCompSolid()) {
      aSelectedShapesType = GeomAPI_Shape::SOLID;
      ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
      if (aResCompSolidPtr.get()) {
        GeomShapePtr aCompSolidShape = aResCompSolidPtr->shape();
        aSelectedCompSolidsInOtherList.bind(aCompSolidShape, aCompSolidShape);
      }
    } else {
      aSelectedShapesType = GeomAPI_Shape::FACE;
      GeomAPI_Face aFace(aShape);
      aFacesPln = aFace.getPlane();
      break;
    }
  }

  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }

    if (anAttrSelection->contextFeature().get()) {
      theError = "Error: Features not allowed for selection.";
      return false;
    }

    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      FeaturePtr aContFeat = anAttrSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size() ||
          aContFeat->firstResult()->groupName() != ModelAPI_ResultBody::group()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if (aResultConstruction.get()) {
      theError = "Error: Result construction not allowed for selection.";
      return false;
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    GeomShapePtr aContextShape = aContext->shape();
    if (!aShape.get()) {
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }
    if (!aShape->isEqual(aContextShape)) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    if (aSelectedShapesType == GeomAPI_Shape::SHAPE) {
      // Other list is empty.
      if (aShape->isSolid() || aShape->isCompSolid()) {
        aSelectedShapesType = GeomAPI_Shape::SOLID;
      } else {
        aSelectedShapesType = GeomAPI_Shape::FACE;
        GeomAPI_Face aFace(aShape);
        aFacesPln = aFace.getPlane();

        if (!aFacesPln.get()) {
          theError = "Error: Only planar faces allowed.";
          return false;
        }
      }

      continue;
    } else if (aSelectedShapesType == GeomAPI_Shape::SOLID) {
      if (!aShape->isSolid() && !aShape->isCompSolid()) {
        theError = "Error: Selected shapes should have the same type.";
        return false;
      }
    } else {
      GeomAPI_Face aFace(aShape);
      GeomPlanePtr aPln = aFace.getPlane();

      if (!aPln.get()) {
        theError = "Error: Only planar faces allowed.";
        return false;
      }

      if (!aFacesPln->isCoincident(aPln)) {
        theError = "Error: Only coincident faces allowed.";
        return false;
      }
    }
  }

  return true;
}

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_IntersectionSelection::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments,
                                                   Events_InfoMessage& theError) const
{
  if (!theAttribute.get()) {
    theError = "Error: empty selection.";
    return false;
  }
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (aContext.get()) {
      aFeature = ModelAPI_Feature::feature(aContext);
    } else {
      aFeature = anAttrSelection->contextFeature();
      if (!aFeature.get() || !aFeature->results().size() ||
          aFeature->firstResult()->groupName() != ModelAPI_ResultBody::group()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    if (!aFeature.get()) {
      theError = "Error: empty feature.";
      return false;
    }
    std::string aFeatureKind = aFeature->getKind();
    if (aFeatureKind == "Sketch" ||
        aFeatureKind == "Plane" ||
        aFeatureKind == "Axis") {
      theError = "Error: %1 shape is not allowed for selection.";
      theError.arg(aFeatureKind);
      return false;
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if (!aShape.get()) {
      GeomShapePtr aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: empty shape.";
      return false;
    }
    if (aContext.get() && !aShape->isEqual(aContext->shape())) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    int aShapeType = aShape->shapeType();
    // Allow to select edges, faces and solids.
    if (aShapeType != GeomAPI_Shape::EDGE &&
        aShapeType != GeomAPI_Shape::FACE &&
        aShapeType != GeomAPI_Shape::SOLID &&
        aShapeType != GeomAPI_Shape::COMPSOLID &&
        aShapeType != GeomAPI_Shape::COMPOUND) {
      theError = "Error: selected shape has the wrong type.";
      return false;
    }
  }

  return true;
}
// LCOV_EXCL_STOP

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorBooleanFuseSelection::isValid(
  const AttributePtr& theAttribute,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    theError =
      "Error: This validator can only work with selection list attributes in \"Boolean\" feature.";
    return false;
  }

  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      FeaturePtr aContFeat = anAttrSelection->contextFeature();
      if (!aContFeat.get() || !aContFeat->results().size() ||
          aContFeat->firstResult()->groupName() != ModelAPI_ResultBody::group()) {
        theError = "Error: Empty selection context.";
        return false;
      }
    }
    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if (aResultConstruction.get()) {
      theError = "Error: Result construction not allowed for selection.";
      return false;
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if (!aShape.get()) {
      GeomShapePtr aContextShape = aContext->shape();
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }
    if (aContext.get() && !aShape->isEqual(aContext->shape())) {
      theError = "Error: Local selection not allowed.";
      return false;
    }
  }

  return true;
}
// LCOV_EXCL_STOP

//=================================================================================================
bool FeaturesPlugin_ValidatorBooleanFuseArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if (theArguments.size() != 2) {
    theError = "Wrong number of arguments (expected 2).";
    return false;
  }
// LCOV_EXCL_STOP

  std::shared_ptr<FeaturesPlugin_BooleanFuse> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_BooleanFuse>(theFeature);

  int anObjectsNb = 0, aToolsNb = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin();

  bool isAllInSameCompSolid = true;
  ResultBodyPtr aCompSolid;

  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
  if (anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
    for (int anIndex = 0; anIndex < anObjectsNb; ++anIndex) {
      AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
      ResultPtr aContext = anAttr->context();
      ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
      if (aResCompSolidPtr.get()) {
        if (aCompSolid.get()) {
          isAllInSameCompSolid = aCompSolid == aResCompSolidPtr;
        } else {
          aCompSolid = aResCompSolidPtr;
        }
      } else {
        isAllInSameCompSolid = false;
        break;
      }
    }
  }
  anIt++;

  if (aFeature->string(FeaturesPlugin_BooleanFuse::CREATION_METHOD())->value()
      == FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED()) {
    anAttrSelList = theFeature->selectionList(*anIt);
    if (anAttrSelList) {
      aToolsNb = anAttrSelList->size();
      if (isAllInSameCompSolid) {
        for (int anIndex = 0; anIndex < aToolsNb; ++anIndex) {
          AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
          ResultPtr aContext = anAttr->context();
          ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
          if (aResCompSolidPtr.get()) {
            if (aCompSolid.get()) {
              isAllInSameCompSolid = aCompSolid == aResCompSolidPtr;
            } else {
              aCompSolid = aResCompSolidPtr;
            }
          } else {
            isAllInSameCompSolid = false;
            break;
          }
        }
      }
    }
  }

  if (anObjectsNb + aToolsNb < 2) {
    theError = "Not enough arguments for Fuse operation.";
    return false;
  }

  return true;
}

//=================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorBooleanFuseArguments::isNotObligatory(
  std::string theFeature,
  std::string theAttribute)
{
  if (theAttribute == "main_objects" || theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
// LCOV_EXCL_STOP

//==================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorBooleanCommonSelection::isValid(
  const AttributePtr& theAttribute,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    theError =
      "Error: This validator can only work with selection list attributes in \"Boolean\" feature.";
    return false;
  }

  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get() && !anAttrSelection->contextFeature().get()) {
      theError = "Error: Empty selection context.";
      return false;
    }
    ResultConstructionPtr aResultConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if (aResultConstruction.get()) {
      if (theAttribute->id() != FeaturesPlugin_BooleanCommon::TOOL_LIST_ID()) {
        theError = "Error: Result construction not allowed for selection.";
        return false;
      }
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    GeomShapePtr aContextShape;
    if (aContext.get()) {
      aContextShape = aContext->shape();
    }
    if (!aShape.get()) {
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }
    if (aContextShape.get() && !aShape->isEqual(aContextShape)) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    if (aResultConstruction.get() && aShape->shapeType() != GeomAPI_Shape::FACE) {
      theError = "Error: Result construction should be plane.";
      return false;
    }
  }

  return true;
}
// LCOV_EXCL_STOP

//=================================================================================================
bool FeaturesPlugin_ValidatorBooleanCommonArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  if (theArguments.size() != 2) {
// LCOV_EXCL_START
    theError = "Wrong number of arguments (expected 2).";
    return false;
// LCOV_EXCL_STOP
  }

  std::shared_ptr<FeaturesPlugin_BooleanCommon> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_BooleanCommon>(theFeature);

  int anObjectsNb = 0, aToolsNb = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin();

  ResultBodyPtr aCompSolid;

  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
  if (anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
  }

  bool isSimpleMode = aFeature->string(FeaturesPlugin_BooleanCommon::CREATION_METHOD())->value()
                      == FeaturesPlugin_BooleanCommon::CREATION_METHOD_SIMPLE();

  if (!isSimpleMode) {
    anAttrSelList = theFeature->selectionList(*anIt);
    if (anAttrSelList) {
      aToolsNb = anAttrSelList->size();
    }
  }

  if ((isSimpleMode && anObjectsNb < 2)
      || (!isSimpleMode && (anObjectsNb == 0 || aToolsNb == 0))) {
    theError = "Not enough arguments for Common operation.";
    return false;
  }
  return true;
}

//=================================================================================================
// LCOV_EXCL_START
bool FeaturesPlugin_ValidatorBooleanCommonArguments::isNotObligatory(
  std::string theFeature,
  std::string theAttribute)
{
  return false;
}
// LCOV_EXCL_STOP

//==================================================================================================
bool FeaturesPlugin_ValidatorDefeaturingSelection::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    // LCOV_EXCL_START
    theError = "Error: This validator can only work with selection list attributes.";
    return false;
    // LCOV_EXCL_STOP
  }

  // Check selected entities are sub-shapes of solid or compsolid
  GeomShapePtr aBaseSolid;
  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Empty selection context.";
      return false;
    }

    GeomShapePtr aContextShape = aContext->shape();
    if (aContextShape->shapeType() != GeomAPI_Shape::SOLID) {
      theError = "Error: Not all selected shapes are sub-shapes of solids.";
      return false;
    }

    ResultBodyPtr aResRootPtr = ModelAPI_Tools::bodyOwner(aContext, true);
    if (aResRootPtr.get() && aResRootPtr->shape().get()) {
      if (!aResRootPtr->shape()->isCollectionOfSolids()) {
        theError = "Error: The main shape should be a collection of solids";
        return false;
      }
    }
  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorSewingSelection::isValid(const AttributePtr& theAttribute,
                                                      const std::list<std::string>& theArguments,
                                                      Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    theError = "Error: This validator can only work with selection list attributes.";
    return false;
  }

  // Check selected entities are of valid types
  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Empty selection context.";
      return false;
    }
    if (aContext->groupName() != ModelAPI_ResultBody::group()) {
      theError = "Error: Not a result body.";
      return false;
    }

    GeomShapePtr aContextShape = aContext->shape();
    if (!aContextShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }

    GeomAPI_Shape::ShapeType aShapeType = aContextShape->shapeType();
    std::set<GeomAPI_Shape::ShapeType> anAllowedTypes;
    anAllowedTypes.insert(GeomAPI_Shape::FACE);
    anAllowedTypes.insert(GeomAPI_Shape::SHELL);
    anAllowedTypes.insert(GeomAPI_Shape::SOLID);
    anAllowedTypes.insert(GeomAPI_Shape::COMPSOLID);
    anAllowedTypes.insert(GeomAPI_Shape::COMPOUND);
    if (anAllowedTypes.find(aShapeType) == anAllowedTypes.end()) {
      theError = "Error: Selected shape has the wrong type.";
      return false;
    }

  }

  return true;
}

//==================================================================================================
bool FeaturesPlugin_ValidatorGlueFacesSelection::isValid(const AttributePtr& theAttribute,
                                                         const std::list<std::string>& theArguments,
                                                         Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!anAttrSelectionList.get()) {
    theError = "Error: This validator can only work with selection list attributes.";
    return false;
  }

  // Check selected entities are of valid types
  for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if (!anAttrSelection.get()) {
      theError = "Error: Empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Empty selection context.";
      return false;
    }
    if (aContext->groupName() != ModelAPI_ResultBody::group()) {
      theError = "Error: Not a result body.";
      return false;
    }

    GeomShapePtr aContextShape = aContext->shape();
    if (!aContextShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }

    GeomAPI_Shape::ShapeType aShapeType = aContextShape->shapeType();
    std::set<GeomAPI_Shape::ShapeType> anAllowedTypes;
    anAllowedTypes.insert(GeomAPI_Shape::FACE);
    anAllowedTypes.insert(GeomAPI_Shape::SHELL);
    anAllowedTypes.insert(GeomAPI_Shape::SOLID);
    anAllowedTypes.insert(GeomAPI_Shape::COMPSOLID);
    anAllowedTypes.insert(GeomAPI_Shape::COMPOUND);
    if (anAllowedTypes.find(aShapeType) == anAllowedTypes.end()) {
      theError = "Error: Selected shape has the wrong type.";
      return false;
    }

  }

  return true;
}

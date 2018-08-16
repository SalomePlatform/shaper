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

#include "FeaturesPlugin_Validators.h"

#include "FeaturesPlugin_Boolean.h"
#include "FeaturesPlugin_BooleanFuse.h"
#include "FeaturesPlugin_BooleanSmash.h"
#include "FeaturesPlugin_Union.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultCompSolid.h>
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
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#define _USE_MATH_DEFINES
#include <math.h>

//==================================================================================================
bool FeaturesPlugin_ValidatorPipePath::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  AttributeSelectionPtr aPathAttrSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  if(!aPathAttrSelection.get()) {
    theError = "Error: This validator can only work with path selector in \"Pipe\" feature.";
    return false;
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
bool FeaturesPlugin_ValidatorPipeLocations::isValid(
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

//==================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
  if(theArguments.empty()) {
    theError = "Error: Validator parameters is empty.";
    return false;
  }

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
      if(!aContext.get()) {
        theError = "Error: Empty context.";
        return false;
      }

      ResultConstructionPtr aResultConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
      if(!aResultConstruction.get()) {
        // It is not a result construction.
        // If shape is compound check that it contains only faces and edges.
        GeomShapePtr aShape = aSelectionAttr->value();
        if(!aShape.get()) {
          aShape = aContext->shape();
        }

        if(aShape->shapeType() == GeomAPI_Shape::COMPOUND) {
          for(GeomAPI_ShapeIterator anIt(aShape); anIt.more(); anIt.next()) {
            GeomShapePtr aSubShape = anIt.current();
            if(aSubShape->shapeType() != GeomAPI_Shape::EDGE
                && aSubShape->shapeType() != GeomAPI_Shape::FACE) {
              theError = "Error: Compound should contain only faces and edges.";
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
              "Error: Objects with such wire already selected. Don't allow to select this object.";
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
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseObjectsID);
    return false;
  }

  std::set<ResultConstructionPtr> aSelectedSketches;
  std::set<ResultConstructionPtr> aSelectedSketchesFromObjects;

  for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
    AttributeSelectionPtr aSelectionAttr = aListAttr->value(anIndex);
    ResultPtr aContext = aSelectionAttr->context();
    if(!aContext.get()) {
      theError = "Error: Empty context.";
      return false;
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
    theError = "Error: Empty attribute.";
    return false;
  }

  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      // If at least one attribute is invalid, the result is false.
      if(!isValidAttribute(aListAttr->value(anIndex), theArguments, theError)) {
        return false;
      }
    }
  } else if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    // Getting context.
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    if(!aContext.get()) {
      theError = "Error: Attribute have empty context.";
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Error: Empty shape selected";
      return false;
    }

    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      // Construciotn selected. Check that is is not infinite.
      if(aConstruction->isInfinite()) {
        theError = "Error: Infinite constructions is not allowed as base.";
        return false;
      }

      if(aShape->isEqual(aContextShape)) {
        // Whole construction selected. Check that it have faces.
        if(aConstruction->facesNum() > 0) {
          return true;
        }
      } else {
        // Shape on construction selected. Check that it is a face or wire.
        if(aShape->shapeType() == GeomAPI_Shape::WIRE ||
           aShape->shapeType() == GeomAPI_Shape::FACE) {
          return true;
        }
      }

      return false;
    }

    if(!aShape->isEqual(aContextShape)) {
      // Local selection on body does not allowed.
      theError =
        "Error: Selected shape is in the local selection. Only global selection is allowed.";
      return false;
    }

    // Check that object is a shape with allowed type.
    GeomValidators_ShapeType aShapeTypeValidator;
    if(!aShapeTypeValidator.isValid(anAttr, theArguments, theError)) {
      theError = "Error: Selected shape has unacceptable type. Acceptable types are: faces or "
                 "wires on sketch, whole sketch(if it has at least one face), "
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
    theError = "Error: Attribute \"%1\" does not supported by this validator.";
    theError.arg(anAttributeType);
    return false;
  }

  return true;
}

//==================================================================================================
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

//==================================================================================================
bool FeaturesPlugin_ValidatorExtrusionDir::isValid(
                                               const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  if(theArguments.size() != 2) {
    theError = "Error: Validator should be used with 2 parameters for extrusion.";
    return false;
  }

  std::list<std::string>::const_iterator
    anArgsIt = theArguments.begin(), aLast = theArguments.end();

  AttributePtr aCheckAttribute = theFeature->attribute(*anArgsIt);
  ++anArgsIt;

  GeomShapePtr aDirShape;
  AttributeSelectionPtr aSelAttr = theFeature->selection(*anArgsIt);
  if(aSelAttr.get()) {
    aDirShape = aSelAttr->value();
    if(!aDirShape.get()) {
      ResultPtr aContext = aSelAttr->context();
      if(aContext.get()) {
        aDirShape = aContext->shape();
      }
    }
  }

  if(!aDirShape.get()) {
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
  if(!theAttribute.get()) {
    return true;
  }

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
    if(!aContext.get()) {
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
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
bool FeaturesPlugin_ValidatorBooleanSelection::isValid(const AttributePtr& theAttribute,
                                                       const std::list<std::string>& theArguments,
                                                       Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!anAttrSelectionList.get()) {
    theError =
      "Error: This validator can only work with selection list attributes in \"Boolean\" feature.";
    return false;
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
      theError = "Error: Empty selection context.";
      return false;
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
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Error: Empty shape.";
      return false;
    }
    if(!aShape->isEqual(aContextShape)) {
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
    theError =
      "Error: This validator can only work with selection list attributes in \"Fillet\" feature.";
    return false;
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
      theError = "Error: Empty selection context.";
      return false;
    }

    ResultCompSolidPtr aContextOwner = ModelAPI_Tools::compSolidOwner(aContext);
    GeomShapePtr anOwner = aContextOwner.get() ? aContextOwner->shape() : aContext->shape();

    if (anOwner->shapeType() != GeomAPI_Shape::SOLID &&
        anOwner->shapeType() != GeomAPI_Shape::COMPSOLID) {
      theError = "Error: Not all selected shapes are sub-shapes of solids";
      return false;
    }

    if (!aBaseSolid)
      aBaseSolid = anOwner;
    else if (!aBaseSolid->isEqual(anOwner)) {
      theError = "Error: Sub-shapes of different solids have been selected.";
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
    theError = "Error: This validator can only work with selection list in \"Partition\" feature.";
    return false;
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

    ResultCompSolidPtr aResultCompsolid =
      std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aContext);
    if(aResultCompsolid.get()) {
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
    theError =
      "Error: This validator can only work with selection list in \"Remove Sub-Shapes\" feature.";
    return false;
  }

  static const std::string aBaseShapeID = "base_shape";
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionPtr aShapeAttrSelection = aFeature->selection(aBaseShapeID);

  if(!aShapeAttrSelection.get()) {
    theError = "Error: Could not get \"%1\" attribute.";
    theError.arg(aBaseShapeID);
    return false;
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

  for(int anIndex = 0; anIndex < aSubShapesAttrList->size(); ++anIndex) {
    bool isSameFound = false;
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
    GeomShapePtr aShapeToAdd = anAttrSelectionInList->value();
    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      if(anIt.current()->isEqual(aShapeToAdd)) {
        isSameFound = true;
        break;
      }
    }
    if(!isSameFound) {
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
    bool isSameFound = false;
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

    ResultCompSolidPtr aResult =
      std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aContext);
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

//==================================================================================================
bool FeaturesPlugin_ValidatorUnionArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
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
  ListOfShape aCombined, aFree;
  GeomAlgoAPI_ShapeTools::combineShapes(
    aCompound,
    aType,
    aCombined,
    aFree);

  if(aFree.size() > 0 || aCombined.size() > 1) {
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
    theError = "Error: The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
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
  if (theArguments.size() != 2)
  {
    theError = "Wrong number of arguments (expected 2).";
    return false;
  }

  int anObjectsNb = 0, aToolsNb = 0;
  //int anOperationType = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  bool isAllInSameCompSolid = true;
  ResultCompSolidPtr aCompSolid;

  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
  if (anAttrSelList)
  {
    anObjectsNb = anAttrSelList->size();
    for (int anIndex = 0; anIndex < anObjectsNb; ++anIndex)
    {
      AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
      ResultPtr aContext = anAttr->context();
      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
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
  anIt++;


  anAttrSelList = theFeature->selectionList(*anIt);
  if (anAttrSelList)
  {
    aToolsNb = anAttrSelList->size();
    if (isAllInSameCompSolid)
    {
      for (int anIndex = 0; anIndex < aToolsNb; ++anIndex)
      {
        AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
        ResultPtr aContext = anAttr->context();
        ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
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
  anIt++;

  std::shared_ptr<FeaturesPlugin_Boolean> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_Boolean>(theFeature);
  FeaturesPlugin_Boolean::OperationType anOperationType = aFeature->operationType();

  if (anOperationType == FeaturesPlugin_Boolean::BOOL_FUSE)
  {
    // Fuse operation
    if (anObjectsNb + aToolsNb < 2)
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
    if (anObjectsNb < 1)
    {
      theError = "Objects not selected.";
      return false;
    }
    if (aToolsNb < 1)
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
bool FeaturesPlugin_ValidatorBooleanArguments::isNotObligatory(std::string theFeature,
                                                               std::string theAttribute)
{
  if (theAttribute == "main_objects" || theAttribute == "tool_objects")
  {
    return true;
  }

  return false;
}

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
    theError =
      "Error: Validator used in wrong feature or attribute";
    return false;
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
    ResultPtr aContext = anAttrSelection->context();
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    GeomShapePtr aContextShape = aContext->shape();
    if (!aShape.get()) {
      aShape = aContextShape;
    }

    if (aShape->isSolid() || aShape->isCompSolid()) {
      aSelectedShapesType = GeomAPI_Shape::SOLID;
      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
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
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Empty selection context.";
      return false;
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

      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
      if (aResCompSolidPtr.get()) {
        GeomShapePtr aCompSolidShape = aResCompSolidPtr->shape();
        if (aSelectedCompSolidsInOtherList.isBound(aCompSolidShape)) {
          theError = "Error: Solids from compsolid in other list not allowed.";
          return false;
        }
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
    if (!aContext.get()) {
      theError = "Error: empty selection context.";
      return false;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
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
    GeomShapePtr aContextShape = aContext->shape();
    if (!aShape.get()) {
      aShape = aContextShape;
    }
    if (!aShape.get()) {
      theError = "Error: empty shape.";
      return false;
    }
    if (!aShape->isEqual(aContextShape)) {
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

//==================================================================================================
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
    if (!aContext.get()) {
      theError = "Error: Empty selection context.";
      return false;
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
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBooleanFuseArguments::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  if (theArguments.size() != 2) {
    theError = "Wrong number of arguments (expected 2).";
    return false;
  }

  std::shared_ptr<FeaturesPlugin_BooleanFuse> aFeature =
    std::dynamic_pointer_cast<FeaturesPlugin_BooleanFuse>(theFeature);

  int anObjectsNb = 0, aToolsNb = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  bool isAllInSameCompSolid = true;
  ResultCompSolidPtr aCompSolid;

  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
  if (anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
    for (int anIndex = 0; anIndex < anObjectsNb; ++anIndex) {
      AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
      ResultPtr aContext = anAttr->context();
      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
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
          ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
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

  anIt++;

  if (anObjectsNb + aToolsNb < 2) {
    theError = "Not enough arguments for Fuse operation.";
    return false;
  } else if (isAllInSameCompSolid) {
    theError = "Operations only between sub-shapes of the same shape not allowed.";
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBooleanFuseArguments::isNotObligatory(
  std::string theFeature,
  std::string theAttribute)
{
  if (theAttribute == "main_objects" || theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
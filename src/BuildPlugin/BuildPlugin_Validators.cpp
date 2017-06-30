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

#include "BuildPlugin_Validators.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomValidators_FeatureKind.h>
#include <GeomValidators_ShapeType.h>

#include <Events_InfoMessage.h>

//=================================================================================================
bool BuildPlugin_ValidatorBaseForBuild::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  // Get base objects list.
  if(theAttribute->attributeType() != ModelAPI_AttributeSelectionList::typeId()) {
    std::string aMsg = "Error: BuildPlugin_ValidatorBaseForBuild does "
                       "not support attribute type '%1'\nOnly '%2' is supported.";
    Events_InfoMessage("BuildPlugin_Validators", aMsg).
      arg(theAttribute->attributeType()).arg(ModelAPI_AttributeSelectionList::typeId()).send();
    return false;
  }
  AttributeSelectionListPtr aSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aSelectionList.get()) {
    theError = "Could not get selection list.";
    return false;
  }
  if(aSelectionList->size() == 0) {
    theError = "Empty selection list.";
    return false;
  }

  // Collect base shapes.
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    if(!aSelection.get()) {
      theError = "Could not get selection.";
      return false;
    }
    ResultPtr aContext = aSelection->context();
    if(!aContext.get()) {
      theError = "Attribute have empty context.";
      return false;
    }

    GeomShapePtr aShape = aSelection->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Empty shape selected.";
      return false;
    }

    // Check that shapes has acceptable type.
    GeomValidators_ShapeType aValidatorShapeType;
    if(!aValidatorShapeType.isValid(aSelection, theArguments, theError)) {
      return false;
    }

    // Check that it is shape on sketch.
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      if(aConstruction->isInfinite()) {
        theError = "Inifinte objects not acceptable.";
        return false;
      }

      std::shared_ptr<GeomAPI_PlanarEdges> anEdges =
        std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
      if(anEdges.get()) {
        if(aShape->isEqual(aContextShape)) {
          // It is whole sketch.
          return false;
        }

        continue;
      }
    }

    if(!aShape->isEqual(aContextShape)) {
      // Local selection on body does not allowed.
      theError = "Selected shape is in the local selection. Only global selection is allowed.";
      return false;
    }
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWire::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  // Get attribute.
  if(theArguments.size() != 1) {
    std::string aMsg = "Error: BuildPlugin_ValidatorBaseForWire should be used only "
                       "with 1 parameter (ID of base objects list).";
    Events_InfoMessage("BuildPlugin_Validators", aMsg).send();
    return false;
  }
  AttributeSelectionListPtr aSelectionList = theFeature->selectionList(theArguments.front());
  if(!aSelectionList.get()) {
    theError = "Empty attribute \"%1\".";
    theError.arg(theArguments.front());
    return false;
  }


  // Collect base shapes.
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      if (aSelection->context().get())
        aShape = aSelection->context()->shape();
    }
    if (aShape.get())
      aListOfShapes.push_back(aShape);
  }

  // Create wire.
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(aListOfShapes);
  if(!aWire.get()) {
    theError = "Result wire empty. Probably it has disconnected edges or non-manifold.";
    return false;
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWire::isNotObligatory(std::string theFeature,
                                                       std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForFace::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  // Get attribute.
  if(theArguments.size() != 1) {
    std::string aMsg = "Error: BuildPlugin_ValidatorBaseForFace should be used only with "
      "1 parameter (ID of base objects list).";
    Events_InfoMessage("BuildPlugin_Validators", aMsg).send();
    return false;
  }
  AttributeSelectionListPtr aSelectionList = theFeature->selectionList(theArguments.front());
  if(!aSelectionList.get()) {
    theError = "Empty attribute \"%1\".";
    theError.arg(theArguments.front());
    return false;
  }

  // Collect base shapes.
  ListOfShape anEdges;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      if (!aSelection->context()) {
        theError = "Objects are not selected.";
        return false;
      }
      aShape = aSelection->context()->shape();
    }
    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      anEdges.push_back(anEdge);
    }
  }

  if(anEdges.empty()) {
    theError = "Objects are not selected.";
    return false;
  }

  // Check that edges does not have intersections.
  if(anEdges.size() > 1) {
    GeomAlgoAPI_PaveFiller aPaveFiller(anEdges, false);
    if(!aPaveFiller.isDone()) {
      theError = "Error while checking if edges intersects.";
      return false;
    }
    GeomShapePtr aSectedEdges = aPaveFiller.shape();

    int anEdgesNum = 0;
    for(GeomAPI_ShapeExplorer
        anExp(aSectedEdges, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      anEdgesNum++;
    }
    if(anEdgesNum != anEdges.size()) {
      theError = "Selected objects have intersections.";
      return false;
    }
  }

  // Check that they are planar.
  std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_ShapeTools::findPlane(anEdges);
  if(!aPln.get()) {
    theError = "Selected object(s) should belong to only one plane.";
    return false;
  }

  // Check that selected objects have closed contours.
  ListOfShape aFaces;
  GeomAlgoAPI_SketchBuilder::createFaces(aPln->location(), aPln->xDirection(),
                                         aPln->direction(), anEdges, aFaces);
  if(aFaces.empty()) {
    theError = "Selected objects do not generate closed contour.";
    return false;
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForFace::isNotObligatory(std::string theFeature,
                                                       std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool BuildPlugin_ValidatorSubShapesSelection::isValid(const AttributePtr& theAttribute,
                                                      const std::list<std::string>& theArguments,
                                                      Events_InfoMessage& theError) const
{
  if(theArguments.size() != 1) {
    std::string aMsg = "Error: BuildPlugin_ValidatorSubShapesSelection should be used only with "
      "1 parameter(Sketch feature id).";
    Events_InfoMessage("BuildPlugin_Validators", aMsg).send();
    return false;
  }

  // Get base objects list.
  if(theAttribute->attributeType() != ModelAPI_AttributeSelectionList::typeId()) {
    std::string aMsg =
      "Error: BuildPlugin_ValidatorSubShapesSelection does not support attribute type \""
      "%1\"\n Only \"%2\" supported.";
    Events_InfoMessage("BuildPlugin_Validators", aMsg).
      arg(theAttribute->attributeType()).arg(ModelAPI_AttributeSelectionList::typeId()).send();
    return false;
  }
  AttributeSelectionListPtr aSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aSelectionList.get()) {
    theError = "Could not get selection list.";
    return false;
  }

  // Get base shape.
  const std::string aBaseShapeId = "base_shape";
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  AttributeSelectionPtr aShapeAttrSelection = aFeature->selection(aBaseShapeId);

  if(!aShapeAttrSelection.get()) {
    theError = "Base shape is empty.";
    return false;
  }

  ResultPtr aBaseContext = aShapeAttrSelection->context();

  GeomShapePtr aBaseShape  = aShapeAttrSelection->value();
  if(!aBaseShape.get()) {
    theError = "Base shape is empty.";
    return false;
  }

  GeomAlgoAPI_ShapeBuilder aBuilder;
  aBuilder.removeInternal(aBaseShape);
  aBaseShape = aBuilder.shape();

  // If selected shape is wire allow to select only vertices. If face - allow vertices and edges.
  std::set<GeomAPI_Shape::ShapeType> anAllowedTypes;
  switch(aBaseShape->shapeType()) {
    case GeomAPI_Shape::FACE: anAllowedTypes.insert(GeomAPI_Shape::EDGE);
    case GeomAPI_Shape::WIRE: anAllowedTypes.insert(GeomAPI_Shape::VERTEX);
    default: break;
  }

  // Check selected shapes.
  GeomValidators_FeatureKind aFeatureKindValidator;
  std::list<std::string> anArguments;
  anArguments.push_back(theArguments.front());
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelectionAttrInList = aSelectionList->value(anIndex);
    if(!aSelectionAttrInList.get()) {
      theError = "Empty attribute in list.";
      return false;
    }

    // If context of selection same skip.
    if(aBaseContext == aSelectionAttrInList->context()) {
      continue;
    }

    // Check that it is a selection on Sketch.
    if(!aFeatureKindValidator.isValid(aSelectionAttrInList, anArguments, theError)) {
      return false;
    }

    // Check shape type.
    GeomShapePtr aShapeInList = aSelectionAttrInList->value();
    if(!aShapeInList.get()) {
      aShapeInList = aSelectionAttrInList->context()->shape();
    }
    if(anAllowedTypes.find(aShapeInList->shapeType()) == anAllowedTypes.cend()) {
      theError = "Selected shape has unacceptable type.";
      return false;
    }

    // Check that shape inside wire or face.
    if(!GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aShapeInList, aBaseShape)) {
      theError = "Selected shape is not inside base face.";
      return false;
    }
  }

  return true;
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Validators.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomValidators_ShapeType.h>

#include <Events_Error.h>

//=================================================================================================
bool BuildPlugin_ValidatorBaseForBuild::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  // Get base objects list.
  if(theAttribute->attributeType() != ModelAPI_AttributeSelectionList::typeId()) {
    Events_Error::send("Error: BuildPlugin_ValidatorBaseForBuild does not support attribute type \"" + theAttribute->attributeType()
      + "\"\n Only \"" + ModelAPI_AttributeSelectionList::typeId() + "\" supported.");
    return false;
  }
  AttributeSelectionListPtr aSelectionList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
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
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      if(aConstruction->isInfinite()) {
        theError = "Inifinte objects not acceptable.";
        return false;
      }

      std::shared_ptr<GeomAPI_PlanarEdges> anEdges = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
      if(anEdges.get()) {
        if(aShape->isEqual(aContextShape)) {
          // It is whole sketch.
          return false;
        }
      } else if(!aShape->isEqual(aContextShape)) {
        // Local selection on body does not allowed.
        theError = "Selected shape is in the local selection. Only global selection is allowed.";
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWire::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  // Get attribute.
  if(theArguments.size() != 1) {
    Events_Error::send("Error: BuildPlugin_ValidatorBaseForWire should be used only with 1 parameter (ID of base objects list).");
    return false;
  }
  AttributeSelectionListPtr aSelectionList = theFeature->selectionList(theArguments.front());
  if(!aSelectionList.get()) {
    theError = "Empty attribute \"" + theArguments.front() + "\".";
    return false;
  }


  // Collect base shapes.
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
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
bool BuildPlugin_ValidatorBaseForWire::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForFace::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  // Get attribute.
  if(theArguments.size() != 1) {
    Events_Error::send("Error: BuildPlugin_ValidatorBaseForFace should be used only with 1 parameter (ID of base objects list).");
    return false;
  }
  AttributeSelectionListPtr aSelectionList = theFeature->selectionList(theArguments.front());
  if(!aSelectionList.get()) {
    theError = "Empty attribute \"" + theArguments.front() + "\".";
    return false;
  }

  // Collect base shapes.
  ListOfShape anEdges;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      anEdges.push_back(anEdge);
    }
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
    for(GeomAPI_ShapeExplorer anExp(aSectedEdges, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
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
    theError = "Selected objects are not planar.";
    return false;
  }

  // Check that selected objects have closed contours.
  ListOfShape aFaces;
  GeomAlgoAPI_SketchBuilder::createFaces(aPln->location(), aPln->xDirection(), aPln->direction(), anEdges, aFaces);
  if(aFaces.empty()) {
    theError = "Selected objects does not have closed contours.";
    return false;
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForFace::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

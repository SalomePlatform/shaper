// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Revolution.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Revolution.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

//=================================================================================================
FeaturesPlugin_Revolution::FeaturesPlugin_Revolution()
{
}

//=================================================================================================
void FeaturesPlugin_Revolution::initAttributes()
{
  initCompositeSketchAttribtues(InitBaseObjectsList);

  data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());

  initCompositeSketchAttribtues(InitSketchLauncher);
}

//=================================================================================================
void FeaturesPlugin_Revolution::execute()
{
  ListOfShape aBaseShapesList;
  ListOfMakeShape aMakeShapesList;

  // Make revolutions.
  if(!makeRevolutions(aBaseShapesList, aMakeShapesList)) {
    return;
  }

  // Store results.
  int aResultIndex = 0;
  ListOfShape::const_iterator aBaseIt = aBaseShapesList.cbegin();
  ListOfMakeShape::const_iterator anAlgoIt = aMakeShapesList.cbegin();
  for(; aBaseIt != aBaseShapesList.cend() && anAlgoIt != aMakeShapesList.cend(); ++aBaseIt, ++anAlgoIt) {
    storeResult(*aBaseIt, *anAlgoIt, aResultIndex++);
  }

  removeResults(aResultIndex);
}

//=================================================================================================
bool FeaturesPlugin_Revolution::makeRevolutions(ListOfShape& theBaseShapes,
                                                ListOfMakeShape& theMakeShapes)
{
  theMakeShapes.clear();

  /// Sub-feature of the composite should be set in the base list.
  setSketchObjectToList();

  // Getting base shapes.
  getBaseShapes(theBaseShapes);

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  AttributeSelectionPtr aSelection = selection(AXIS_OBJECT_ID());
  if(aSelection.get() && aSelection->value().get() && aSelection->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aSelection->value()));
  } else if(aSelection->context().get() &&
            aSelection->context()->shape().get() &&
            aSelection->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aSelection->context()->shape()));
  }
  if(anEdge.get()) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                          anEdge->line()->direction()));
  }

  // Getting angles.
  double aToAngle = 0.0;
  double aFromAngle = 0.0;

  if(string(CREATION_METHOD())->value() == "ByAngles") {
    aToAngle = real(TO_ANGLE_ID())->value();
    aFromAngle = real(FROM_ANGLE_ID())->value();
  } else {
    aToAngle = real(TO_OFFSET_ID())->value();
    aFromAngle = real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  GeomShapePtr aToShape;
  GeomShapePtr aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    aSelection = selection(TO_OBJECT_ID());
    if(aSelection.get()) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aSelection->value());
      if(!aToShape.get() && aSelection->context().get()) {
        aToShape = aSelection->context()->shape();
      }
    }
    aSelection = selection(FROM_OBJECT_ID());
    if(aSelection.get()) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aSelection->value());
      if(!aFromShape.get() && aSelection->context().get()) {
        aFromShape = aSelection->context()->shape();
      }
    }
  }

  // Generating result for each base shape.
  for(ListOfShape::const_iterator anIter = theBaseShapes.cbegin(); anIter != theBaseShapes.cend(); anIter++) {
    GeomShapePtr aBaseShape = *anIter;

    std::shared_ptr<GeomAlgoAPI_Revolution> aRevolAlgo(new GeomAlgoAPI_Revolution(aBaseShape, anAxis,
                                                                                  aToShape, aToAngle,
                                                                                  aFromShape, aFromAngle));
    if(!isMakeShapeValid(aRevolAlgo)) {
      return false;
    }

    theMakeShapes.push_back(aRevolAlgo);
  }

  return true;
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Prism.h>

//=================================================================================================
FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

//=================================================================================================
void FeaturesPlugin_Extrusion::initAttributes()
{
  initCompositeSketchAttribtues(InitBaseObjectsList);

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(DIRECTION_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), DIRECTION_OBJECT_ID());

  initCompositeSketchAttribtues(InitSketchLauncher);
}

//=================================================================================================
void FeaturesPlugin_Extrusion::execute()
{
  ListOfShape aBaseShapesList;
  ListOfMakeShape aMakeShapesList;

  // Make extrusions.
  if(!makeExtrusions(aBaseShapesList, aMakeShapesList)) {
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
bool FeaturesPlugin_Extrusion::makeExtrusions(ListOfShape& theBaseShapes,
                                              ListOfMakeShape& theMakeShapes)
{
  theMakeShapes.clear();

  /// Sub-feature of the composite should be set in the base list.
  setSketchObjectToList();

  // Getting base shapes.
  getBaseShapes(theBaseShapes);

  // Getting sizes.
  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(string(CREATION_METHOD())->value() == "BySizes") {
    aToSize = real(TO_SIZE_ID())->value();
    aFromSize = real(FROM_SIZE_ID())->value();
  } else {
    aToSize = real(TO_OFFSET_ID())->value();
    aFromSize = real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  GeomShapePtr aToShape;
  GeomShapePtr aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    AttributeSelectionPtr aSelection = selection(TO_OBJECT_ID());
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
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;

    std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo(new GeomAlgoAPI_Prism(aBaseShape,
                                                                        aToShape, aToSize,
                                                                        aFromShape, aFromSize));
    if(!isMakeShapeValid(aPrismAlgo)) {
      return false;
    }

    theMakeShapes.push_back(aPrismAlgo);
  }

  return true;
}

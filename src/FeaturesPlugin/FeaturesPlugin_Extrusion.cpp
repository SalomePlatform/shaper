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

#include "FeaturesPlugin_Extrusion.h"
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>

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
  ListOfShape aBaseShapesList, aBoundaryShapes;
  ListOfMakeShape aMakeShapesList;

  // Make extrusions.
  if(!makeExtrusions(aBaseShapesList, aBoundaryShapes, aMakeShapesList)) {
    return;
  }

  // Store results.
  int aResultIndex = 0;
  ListOfShape::const_iterator aBaseIt = aBaseShapesList.cbegin();
  ListOfMakeShape::const_iterator anAlgoIt = aMakeShapesList.cbegin();
  for(; aBaseIt != aBaseShapesList.cend() && anAlgoIt != aMakeShapesList.cend();
        ++aBaseIt, ++anAlgoIt) {
    if (aBoundaryShapes.empty())
      storeResult(*aBaseIt, *anAlgoIt, aResultIndex++);
    else
      storeResultWithBoundaries(*aBaseIt, aBoundaryShapes, *anAlgoIt, aResultIndex++);
  }

  removeResults(aResultIndex);
}

//=================================================================================================
bool FeaturesPlugin_Extrusion::makeExtrusions(ListOfShape& theBaseShapes,
                                              ListOfShape& theBoundaryShapes,
                                              ListOfMakeShape& theMakeShapes)
{
  theMakeShapes.clear();

  // Getting base shapes.
  getBaseShapes(theBaseShapes);

  //Getting direction.
  std::shared_ptr<GeomAPI_Dir> aDir;
  getDirection(aDir);

  // Getting sizes.
  double aToSize = 0.0;
  double aFromSize = 0.0;
  getSizes(aToSize, aFromSize);

  // Getting bounding planes.
  GeomShapePtr aToShape;
  GeomShapePtr aFromShape;

  if(string(CREATION_METHOD())->value() == CREATION_METHOD_BY_PLANES()) {
    AttributeSelectionPtr aSelection = selection(TO_OBJECT_ID());
    if(aSelection.get()) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aSelection->value());
      if(!aToShape.get() && aSelection->context().get()) {
        aToShape = aSelection->context()->shape();
      }
      if (aToShape.get() && aToShape->isCompound()) {
        GeomAPI_ShapeIterator anIt(aToShape);
        aToShape = anIt.current();
      }
    }
    aSelection = selection(FROM_OBJECT_ID());
    if(aSelection.get()) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aSelection->value());
      if(!aFromShape.get() && aSelection->context().get()) {
        aFromShape = aSelection->context()->shape();
      }
      if (aFromShape.get() && aFromShape->isCompound()) {
        GeomAPI_ShapeIterator anIt(aFromShape);
        aFromShape = anIt.current();
      }
    }
  }
  if (aToShape && !aToShape->isPlanar())
    theBoundaryShapes.push_back(aToShape);
  if (aFromShape && !aFromShape->isPlanar())
    theBoundaryShapes.push_back(aFromShape);

  // Generating result for each base shape.
  std::string anError;
  for(ListOfShape::const_iterator
      anIter = theBaseShapes.cbegin(); anIter != theBaseShapes.cend(); anIter++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;

    std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo(new GeomAlgoAPI_Prism(aBaseShape, aDir,
                                                                        aToShape, aToSize,
                                                                        aFromShape, aFromSize));
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPrismAlgo, getKind(), anError)) {
      setError(anError);
      return false;
    }

    theMakeShapes.push_back(aPrismAlgo);
  }

  return true;
}

//=================================================================================================
void FeaturesPlugin_Extrusion::storeResultWithBoundaries(
    const GeomShapePtr theBaseShape,
    const ListOfShape& theBoundaryShapes,
    const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
    const int theIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store modified shapes.
  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, theBoundaryShapes, ListOfShape(),
                                           theMakeShape, theMakeShape->shape());

  // Store generated edges/faces.
  storeGenerationHistory(aResultBody, theBaseShape, theMakeShape);

  setResult(aResultBody, theIndex);
}

//=================================================================================================
void FeaturesPlugin_Extrusion::getDirection(std::shared_ptr<GeomAPI_Dir>& theDir)
{
  static const std::string aSelectionError = "Error: The direction shape selection is bad.";
  AttributeSelectionPtr aSelection = selection(DIRECTION_OBJECT_ID());
  GeomShapePtr aShape = aSelection->value();
  if (!aShape.get()) {
    if (aSelection->context().get()) {
      aShape = aSelection->context()->shape();
    }
  }

  GeomEdgePtr anEdge;
  if (aShape.get()) {
    if (aShape->isEdge())
    {
      anEdge = aShape->edge();
    }
    else if (aShape->isCompound())
    {
      GeomAPI_ShapeIterator anIt(aShape);
      anEdge = anIt.current()->edge();
    }
  }

  if (anEdge.get()) {
    if (anEdge->isLine()) {
      theDir = anEdge->line()->direction();
    }
  }
}

//=================================================================================================
void FeaturesPlugin_Extrusion::getSizes(double& theToSize, double& theFromSize)
{
  if (string(CREATION_METHOD())->value() == CREATION_METHOD_BY_SIZES()) {
    theToSize = real(TO_SIZE_ID())->value();
    theFromSize = real(FROM_SIZE_ID())->value();
  } if (string(CREATION_METHOD())->value() == CREATION_METHOD_BY_PLANES()) {
    theToSize = real(TO_OFFSET_ID())->value();
    theFromSize = real(FROM_OFFSET_ID())->value();
  } else {
  }
}

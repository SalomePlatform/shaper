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

#include "FeaturesPlugin_Revolution.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Revolution.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>

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
  for(; aBaseIt != aBaseShapesList.cend() && anAlgoIt != aMakeShapesList.cend();
        ++aBaseIt, ++anAlgoIt) {
    storeResult(*aBaseIt, *anAlgoIt, aResultIndex++);
  }

  removeResults(aResultIndex);
}

//=================================================================================================
bool FeaturesPlugin_Revolution::makeRevolutions(ListOfShape& theBaseShapes,
                                                ListOfMakeShape& theMakeShapes)
{
  theMakeShapes.clear();

  // Getting base shapes.
  getBaseShapes(theBaseShapes);

  // Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr aSelection = selection(AXIS_OBJECT_ID());
  GeomShapePtr aShape = aSelection->value();
  if (!aShape.get()) {
    if (aSelection->context().get()) {
      aShape = aSelection->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return false;
  }

  GeomEdgePtr anEdge;
  if (aShape->isEdge())
  {
    anEdge = aShape->edge();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }
  else
  {
    setError(aSelectionError);
    return false;
  }

  std::shared_ptr<GeomAPI_Ax1> anAxis;
  if(anEdge.get()) {
    if(anEdge->isLine()) {
      anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                            anEdge->line()->direction()));
    }
  }

  if(!anAxis.get()) {
    return false;
  }

  // Getting angles.
  double aToAngle = 0.0;
  double aFromAngle = 0.0;

  if(string(CREATION_METHOD())->value() == CREATION_METHOD_BY_ANGLES()) {
    aToAngle = real(TO_ANGLE_ID())->value();
    aFromAngle = real(FROM_ANGLE_ID())->value();
  } else {
    aToAngle = real(TO_OFFSET_ID())->value();
    aFromAngle = real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  GeomShapePtr aToShape;
  GeomShapePtr aFromShape;

  if(string(CREATION_METHOD())->value() == CREATION_METHOD_BY_PLANES()) {
    aSelection = selection(TO_OBJECT_ID());
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

  // Generating result for each base shape.
  for(ListOfShape::const_iterator
      anIter = theBaseShapes.cbegin(); anIter != theBaseShapes.cend(); anIter++) {
    GeomShapePtr aBaseShape = *anIter;

    std::shared_ptr<GeomAlgoAPI_Revolution> aRevolAlgo(new GeomAlgoAPI_Revolution(
                                                       aBaseShape, anAxis,
                                                       aToShape, aToAngle,
                                                       aFromShape, aFromAngle));
    if(!isMakeShapeValid(aRevolAlgo)) {
      return false;
    }

    theMakeShapes.push_back(aRevolAlgo);
  }

  return true;
}

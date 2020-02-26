// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Chamfer.h"
////#include "FeaturesPlugin_Tools.h"
////
////#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
////#include <ModelAPI_ResultBody.h>
////#include <ModelAPI_Session.h>
////#include <ModelAPI_Tools.h>
////#include <ModelAPI_Validator.h>
////
////#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Chamfer.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_Tools.h>
////
////#include <GeomAPI_DataMapOfShapeMapOfShapes.h>
#include <GeomAPI_ShapeExplorer.h>
////#include <GeomAPI_ShapeIterator.h>
////
////// Obtain all sub-shapes from the shape and append them to the list
////static void collectSubs(const GeomShapePtr& theShape,
////                              ListOfShape& theSubs,
////                        const GeomAPI_Shape::ShapeType theShapeType)
////{
////  GeomAPI_ShapeExplorer anExp(theShape, theShapeType);
////  for (; anExp.more(); anExp.next()) {
////    GeomShapePtr aShape = anExp.current();
////    // Store all shapes with FORWARD orientation to avoid duplication of shared edges/vertices
////    aShape->setOrientation(GeomAPI_Shape::FORWARD);
////    theSubs.push_back(aShape);
////  }
////}

// Extract edges from the list
static void extractEdgesAndFaces(const ListOfShape& theShapes,
                                 ListOfShape& theEdges,
                                 std::map<GeomShapePtr, GeomShapePtr>& theMapEdgeFace)
{
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    if ((*anIt)->isEdge())
      theEdges.push_back(*anIt);
    else {
      for (GeomAPI_ShapeExplorer anExp(*anIt, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
        GeomShapePtr aCurrent = anExp.current();
        theEdges.push_back(aCurrent);
        theMapEdgeFace[aCurrent] = *anIt;
      }
    }
}


FeaturesPlugin_Chamfer::FeaturesPlugin_Chamfer()
{
}

void FeaturesPlugin_Chamfer::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());
  AttributePtr aSelectionList = data()->addAttribute(FeaturesPlugin_Chamfer::OBJECT_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D1_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D2_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  initVersion(aSelectionList);
}

AttributePtr FeaturesPlugin_Chamfer::objectsAttribute()
{
  return attribute(OBJECT_LIST_ID());
}

const std::string& FeaturesPlugin_Chamfer::modifiedShapePrefix() const
{
  static const std::string& THE_PREFIX("Chamfer");
  return THE_PREFIX;
}

GeomMakeShapePtr FeaturesPlugin_Chamfer::performOperation(const GeomShapePtr& theSolid,
                                                          const ListOfShape& theEdges)
{
  AttributeStringPtr aCreationMethod = string(CREATION_METHOD());
  if (!aCreationMethod)
    return GeomMakeShapePtr();

  bool isDistDist = aCreationMethod->value() == CREATION_METHOD_DISTANCE_DISTANCE();
  double aD1 = 0.0, aD2 = 0.0, aD = 0.0, anAngle = 0.0;
  if (isDistDist) {
    aD1 = real(FeaturesPlugin_Chamfer::D1_ID())->value();
    aD2 = real(FeaturesPlugin_Chamfer::D2_ID())->value();
  }
  else {
    aD = real(FeaturesPlugin_Chamfer::D_ID())->value();
    anAngle = real(FeaturesPlugin_Chamfer::ANGLE_ID())->value();
  }

  // Perform chamfer operation
  std::shared_ptr<GeomAlgoAPI_Chamfer> aChamferBuilder;
  std::string anError;

  ListOfShape aChamferEdges;
  std::map<GeomShapePtr, GeomShapePtr> aMapEdgeFace;
  extractEdgesAndFaces(theEdges, aChamferEdges, aMapEdgeFace);

  if (isDistDist) {
    aChamferBuilder.reset(new GeomAlgoAPI_Chamfer(
        theSolid, aChamferEdges, aMapEdgeFace, true, aD1, aD2));
  }
  else {
    aChamferBuilder.reset(new GeomAlgoAPI_Chamfer(
        theSolid, aChamferEdges, aMapEdgeFace, false, aD, anAngle));
  }

  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aChamferBuilder, getKind(), anError)) {
    setError(anError);
    return GeomMakeShapePtr();
  }
  return aChamferBuilder;
}

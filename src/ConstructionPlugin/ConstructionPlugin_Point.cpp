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

#include "ConstructionPlugin_Point.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pln.h>

//==================================================================================================
ConstructionPlugin_Point::ConstructionPlugin_Point()
{
}

//==================================================================================================
const std::string& ConstructionPlugin_Point::getKind()
{
  static std::string MY_KIND = ConstructionPlugin_Point::ID();
  return MY_KIND;
}

//==================================================================================================
void ConstructionPlugin_Point::initAttributes()
{
  data()->addAttribute(X(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(Y(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(Z(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(INTERSECTION_LINE_1(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_LINE_2(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(INTERSECTION_LINE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_PLANE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(USE_OFFSET(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(OFFSET(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSE_OFFSET(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(EDGE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(OFFSET_TYPE(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(DISTANCE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(RATIO(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSE(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(POINT_TO_PROJECT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PROJECTION_TYPE(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EDGE_FOR_POINT_PROJECTION(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FACE_FOR_POINT_PROJECTION(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(INTERSECTION_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(INTERSECTION_PLANE_1(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_PLANE_2(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_PLANE_3(), ModelAPI_AttributeSelection::typeId());
}

//==================================================================================================
void ConstructionPlugin_Point::execute()
{
  GeomShapePtr aShape;

  // to support compatibility with old documents where aCreationMethod did not exist
  std::string aCreationMethod =
    string(CREATION_METHOD()).get() && !string(CREATION_METHOD())->value().empty() ?
    string(CREATION_METHOD())->value() : CREATION_METHOD_BY_XYZ();
  if(aCreationMethod == CREATION_METHOD_BY_XYZ()) {
    aShape = createByXYZ();
  } else if(aCreationMethod == CREATION_METHOD_BY_DISTANCE_ON_EDGE()) {
    aShape = createByDistanceOnEdge();
  } else if(aCreationMethod == CREATION_METHOD_BY_PROJECTION()) {
    if (string(PROJECTION_TYPE())->value() == PROJECTION_TYPE_ON_EDGE()) {
      aShape = createByProjectionOnEdge();
    } else {
      aShape = createByProjectionOnFace();
    }
  } else if(aCreationMethod == CREATION_METHOD_BY_INTERSECTION()) {
    std::string anIntersectionType = string(INTERSECTION_TYPE())->value();
    if (anIntersectionType == INTERSECTION_TYPE_BY_LINES()) {
      aShape = createByLinesIntersection();
    } else if (anIntersectionType == INTERSECTION_TYPE_BY_LINE_AND_PLANE()) {
      // this may produce several points
      std::list<std::shared_ptr<GeomAPI_Vertex> > aPoints = createByLineAndPlaneIntersection();
      if (!aPoints.empty()) { // if no points found produce the standard error later
        int anIndex = 0;
        std::list<std::shared_ptr<GeomAPI_Vertex> >::iterator aPIter = aPoints.begin();
        for (; aPIter != aPoints.end(); aPIter++, anIndex++) {
          std::shared_ptr<ModelAPI_ResultConstruction> aConstr =
            document()->createConstruction(data(), anIndex);
          aConstr->setShape(*aPIter);
          setResult(aConstr, anIndex);
        }
        removeResults(anIndex);
        return;
      }
    } else {
      aShape = createByPlanesIntersection();
    }
  }

  if(!aShape.get()) {
    setError("Error: intersection not found.");
    return;
  }

  removeResults(1); // for case the point type was switched from multi-results type
  std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aShape);
  setResult(aConstr);
}

//==================================================================================================
bool ConstructionPlugin_Point::customisePresentation(ResultPtr theResult,
                                                     AISObjectPtr thePrs,
                                                std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
{
  bool isCustomized = theDefaultPrs.get() != NULL &&
                      theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
  //thePrs->setPointMarker(1, 1.); // Set point as a '+' symbol
  return true;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByXYZ()
{
  return GeomAlgoAPI_PointBuilder::vertex(real(X())->value(),
                                          real(Y())->value(),
                                          real(Z())->value());
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByDistanceOnEdge()
{
  // Get edge.
  AttributeSelectionPtr anEdgeSelection = selection(EDGE());
  GeomShapePtr aShape = anEdgeSelection->value();
  if(!aShape.get()) {
    aShape = anEdgeSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShape));

  // Get distance value and percent flag.
  double aValue;
  bool anIsPercent = false;
  if (string(OFFSET_TYPE())->value() == OFFSET_TYPE_BY_DISTANCE()) {
    aValue = real(DISTANCE())->value();
    anIsPercent = false;
  } else {
    aValue = real(RATIO())->value() * 100.0;
    anIsPercent = true;
  }

  // Get reverse flag.
  bool anIsReverse = boolean(REVERSE())->value();

  return GeomAlgoAPI_PointBuilder::vertexOnEdge(anEdge, aValue, anIsPercent, anIsReverse);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByProjectionOnEdge()
{
  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT_TO_PROJECT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if (!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));

  // Get edge.
  AttributeSelectionPtr anEdgeSelection = selection(EDGE_FOR_POINT_PROJECTION());
  GeomShapePtr anEdgeShape = anEdgeSelection->value();
  if (!anEdgeShape.get()) {
    anEdgeShape = anEdgeSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(anEdgeShape));

  return GeomAlgoAPI_PointBuilder::vertexByProjection(aVertex, anEdge);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByProjectionOnFace()
{
  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT_TO_PROJECT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));

  // Get plane.
  AttributeSelectionPtr aPlaneSelection = selection(FACE_FOR_POINT_PROJECTION());
  GeomShapePtr aPlaneShape = aPlaneSelection->value();
  if(!aPlaneShape.get()) {
    aPlaneShape = aPlaneSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aPlaneShape));

  return GeomAlgoAPI_PointBuilder::vertexByProjection(aVertex, aFace);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByLinesIntersection()
{
  // Get first line.
  AttributeSelectionPtr aFirstLineSelection= selection(INTERSECTION_LINE_1());
  GeomShapePtr aFirstLineShape = aFirstLineSelection->value();
  if(!aFirstLineShape.get()) {
    aFirstLineShape = aFirstLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aFirstEdge(new GeomAPI_Edge(aFirstLineShape));

  // Get second line.
  AttributeSelectionPtr aSecondLineSelection= selection(INTERSECTION_LINE_2());
  GeomShapePtr aSecondLineShape = aSecondLineSelection->value();
  if(!aSecondLineShape.get()) {
    aSecondLineShape = aSecondLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aSecondEdge(new GeomAPI_Edge(aSecondLineShape));

  return GeomAlgoAPI_PointBuilder::vertexByIntersection(aFirstEdge, aSecondEdge);
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Vertex> >
  ConstructionPlugin_Point::createByLineAndPlaneIntersection()
{
  // Get line.
  AttributeSelectionPtr aLineSelection = selection(INTERSECTION_LINE());
  GeomShapePtr aLineShape = aLineSelection->value();
  if(!aLineShape.get()) {
    aLineShape = aLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aLineShape));

  // Get plane.
  AttributeSelectionPtr aPlaneSelection= selection(INTERSECTION_PLANE());
  GeomShapePtr aPlaneShape = aPlaneSelection->value();
  if(!aPlaneShape.get()) {
    aPlaneShape = aPlaneSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aPlaneShape));

  if (!string(USE_OFFSET())->value().empty()) {
    double anOffset = real(OFFSET())->value();
    if (boolean(REVERSE_OFFSET())->value())
      anOffset = -anOffset;
    if (fabs(anOffset) > 1.e-9) { // move face
      aFace->translate(aFace->getPlane()->direction(), anOffset);
    }
  }

  return GeomAlgoAPI_ShapeTools::intersect(anEdge, aFace,
    aPlaneSelection->context()->groupName() == ModelAPI_ResultConstruction::group());
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByPlanesIntersection()
{
  // Get plane.
  AttributeSelectionPtr aPlaneSelection1 = selection(INTERSECTION_PLANE_1());
  GeomShapePtr aPlaneShape1 = aPlaneSelection1->value();
  if (!aPlaneShape1.get()) {
    aPlaneShape1 = aPlaneSelection1->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace1(new GeomAPI_Face(aPlaneShape1));
  std::shared_ptr<GeomAPI_Pln> aPln1 = aFace1->getPlane();

  // Get plane.
  AttributeSelectionPtr aPlaneSelection2 = selection(INTERSECTION_PLANE_2());
  GeomShapePtr aPlaneShape2 = aPlaneSelection2->value();
  if (!aPlaneShape2.get()) {
    aPlaneShape2 = aPlaneSelection2->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace2(new GeomAPI_Face(aPlaneShape2));
  std::shared_ptr<GeomAPI_Pln> aPln2 = aFace2->getPlane();

  // Get plane.
  AttributeSelectionPtr aPlaneSelection3 = selection(INTERSECTION_PLANE_3());
  GeomShapePtr aPlaneShape3 = aPlaneSelection3->value();
  if (!aPlaneShape3.get()) {
    aPlaneShape3 = aPlaneSelection3->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace3(new GeomAPI_Face(aPlaneShape3));
  std::shared_ptr<GeomAPI_Pln> aPln3 = aFace3->getPlane();

  std::shared_ptr<GeomAPI_Vertex> aVertex;

  std::shared_ptr<GeomAPI_Lin> anIntersectLine = aPln1->intersect(aPln2);
  if (!anIntersectLine.get()) {
    return aVertex;
  }

  std::shared_ptr<GeomAPI_Pnt> aPnt = aPln3->intersect(anIntersectLine);
  if (aPnt.get()) {
    aVertex.reset(new GeomAPI_Vertex(aPnt->x(), aPnt->y(), aPnt->z()));
  }

  return aVertex;
}

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

  /*data()->addAttribute(EDGE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(DISTANCE_VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(DISTANCE_PERCENT(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(REVERSE(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(POINT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PLANE(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FIRST_LINE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(SECOND_LINE(), ModelAPI_AttributeSelection::typeId());
*/
  data()->addAttribute(INTERSECTION_LINE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(INTERSECTION_PLANE(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(USE_OFFSET(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(OFFSET(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSE_OFFSET(), ModelAPI_AttributeBoolean::typeId());
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
  }/* else if(aCreationMethod == CREATION_METHOD_BY_DISTANCE_ON_EDGE()) {
    aShape = createByDistanceOnEdge();
  } else if(aCreationMethod == CREATION_METHOD_BY_PROJECTION()) {
    aShape = createByProjection();
  } else if(aCreationMethod == CREATION_METHOD_BY_LINES_INTERSECTION()) {
    aShape = createByLinesIntersection();
  }*/ else if(aCreationMethod == CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION()) {
    aShape = createByLineAndPlaneIntersection();
  }

  if(!aShape.get()) {
    setError("Error: intersection not found.");
    return;
  }

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

/*//==================================================================================================
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
  double aValue = real(DISTANCE_VALUE())->value();
  bool anIsPercent = boolean(DISTANCE_PERCENT())->value();

  // Get reverse flag.
  bool anIsReverse = boolean(REVERSE())->value();

  return GeomAlgoAPI_PointBuilder::vertexOnEdge(anEdge, aValue, anIsPercent, anIsReverse);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByProjection()
{
  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));

  // Get plane.
  AttributeSelectionPtr aPlaneSelection = selection(PLANE());
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
  AttributeSelectionPtr aFirstLineSelection= selection(FIRST_LINE());
  GeomShapePtr aFirstLineShape = aFirstLineSelection->value();
  if(!aFirstLineShape.get()) {
    aFirstLineShape = aFirstLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aFirstEdge(new GeomAPI_Edge(aFirstLineShape));

  // Get second line.
  AttributeSelectionPtr aSecondLineSelection= selection(SECOND_LINE());
  GeomShapePtr aSecondLineShape = aSecondLineSelection->value();
  if(!aSecondLineShape.get()) {
    aSecondLineShape = aSecondLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aSecondEdge(new GeomAPI_Edge(aSecondLineShape));

  return GeomAlgoAPI_PointBuilder::vertexByIntersection(aFirstEdge, aSecondEdge);
}
*/

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByLineAndPlaneIntersection()
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

  return GeomAlgoAPI_ShapeTools::intersect(anEdge, aFace);
}

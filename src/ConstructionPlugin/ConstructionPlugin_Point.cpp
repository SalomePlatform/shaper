// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Point.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "ConstructionPlugin_Point.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>

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

  data()->addAttribute(EDGE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(DISTANCE_VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(DISTANCE_PERCENT(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(REVERSE(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(POINT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PLANE(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FIRST_LINE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(SECOND_LINE(), ModelAPI_AttributeSelection::typeId());
}

//==================================================================================================
void ConstructionPlugin_Point::execute()
{
  GeomShapePtr aShape;

  std::string aCreationMethod = string(CREATION_METHOD())->value();
  if(aCreationMethod == CREATION_METHOD_BY_XYZ()) {
    aShape = createByXYZ();
  } else if(aCreationMethod == CREATION_METHOD_BY_DISTANCE_ON_EDGE()) {
    aShape = createByDistanceOnEdge();
  } else if(aCreationMethod == CREATION_METHOD_BY_PROJECTION()) {
    aShape = createByProjection();
  } else if(aCreationMethod == CREATION_METHOD_BY_LINES_INTERSECTION()) {
    aShape = createByIntersection();
  }

  if(aShape.get()) {
    std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
    aConstr->setShape(aShape);
    setResult(aConstr);
  }
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
std::shared_ptr<GeomAPI_Vertex> ConstructionPlugin_Point::createByIntersection()
{
  // Get first line.
  AttributeSelectionPtr aFirstLineSelection= selection(FIRST_LINE());
  GeomShapePtr aFirstLineShape = aFirstLineSelection->value();
  if(!aFirstLineShape.get()) {
    aFirstLineShape = aFirstLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aFirstEdge(new GeomAPI_Edge(aFirstLineShape));

  // Get first line.
  AttributeSelectionPtr aSecondLineSelection= selection(SECOND_LINE());
  GeomShapePtr aSecondLineShape = aSecondLineSelection->value();
  if(!aSecondLineShape.get()) {
    aSecondLineShape = aSecondLineSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> aSecondEdge(new GeomAPI_Edge(aSecondLineShape));

  return GeomAlgoAPI_PointBuilder::vertexByIntersection(aFirstEdge, aSecondEdge);
}

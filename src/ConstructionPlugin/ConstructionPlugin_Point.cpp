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
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>

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
GeomShapePtr ConstructionPlugin_Point::createByXYZ()
{
  return GeomAlgoAPI_PointBuilder::point(real(X())->value(),
                                         real(Y())->value(),
                                         real(Z())->value());
}

//==================================================================================================
GeomShapePtr ConstructionPlugin_Point::createByDistanceOnEdge()
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

  return GeomAlgoAPI_ShapeTools::findVertexOnEdge(anEdge, aValue, anIsPercent, anIsReverse);
}

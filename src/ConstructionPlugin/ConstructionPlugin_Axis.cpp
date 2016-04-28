// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Axis.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Axis.h"

#include <Config_PropManager.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;

ConstructionPlugin_Axis::ConstructionPlugin_Axis()
{
}

void ConstructionPlugin_Axis::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Axis::METHOD(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::POINT_FIRST(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::POINT_SECOND(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::CYLINDRICAL_FACE(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::X_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::Y_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::Z_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), 
      ConstructionPlugin_Axis::X_DIRECTION());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), 
      ConstructionPlugin_Axis::Y_DIRECTION());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), 
      ConstructionPlugin_Axis::Z_DIRECTION());
}

void ConstructionPlugin_Axis::createAxisByTwoPoints()
{
  AttributeSelectionPtr aRef1 = data()->selection(ConstructionPlugin_Axis::POINT_FIRST());
  AttributeSelectionPtr aRef2 = data()->selection(ConstructionPlugin_Axis::POINT_SECOND());
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get())
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    if (aShape1->isVertex() && aShape2->isVertex() && (!aShape1->isEqual(aShape2))) {
      std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> anEnd = GeomAlgoAPI_PointBuilder::point(aShape2);
      if (aStart->distance(anEnd) > ConstructionPlugin_Axis::MINIMAL_LENGTH()) {
        std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

        ResultConstructionPtr aConstr = document()->createConstruction(data());
        aConstr->setInfinite(true);
        aConstr->setShape(anEdge);
        setResult(aConstr);
      }
    }
  }
}


void ConstructionPlugin_Axis::createAxisByPointAndDirection()
{
  AttributeSelectionPtr aRef1 = data()->selection(ConstructionPlugin_Axis::POINT_FIRST());
  AttributeDoublePtr aXAttr = data()->real(ConstructionPlugin_Axis::X_DIRECTION());
  AttributeDoublePtr aYAttr = data()->real(ConstructionPlugin_Axis::Y_DIRECTION());
  AttributeDoublePtr aZAttr = data()->real(ConstructionPlugin_Axis::Z_DIRECTION());
  if ((aRef1.get() != NULL) && (aXAttr.get() != NULL) && 
      (aYAttr.get() != NULL) && (aZAttr.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get())
      aShape1 = aRef1->context()->shape();

    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aXAttr->value(), 
                                                               aYAttr->value(),
                                                               aZAttr->value()));
    if (aShape1->isVertex() && (!aShape1->isEqual(aVertex))) {
      std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> anEnd = aVertex->point();
      if (aStart->distance(anEnd) > ConstructionPlugin_Axis::MINIMAL_LENGTH()) {
        std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

        ResultConstructionPtr aConstr = document()->createConstruction(data());
        aConstr->setInfinite(true);
        aConstr->setShape(anEdge);
        setResult(aConstr);
      }
    }
  }
}


void ConstructionPlugin_Axis::createAxisByCylindricalFace()
{
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(CYLINDRICAL_FACE())->value();
     // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isFace()) {
      std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::cylinderAxis(aSelection);

      ResultConstructionPtr aConstr = document()->createConstruction(data());
      aConstr->setInfinite(true);
      aConstr->setShape(anEdge);
      setResult(aConstr);
    }
}



void ConstructionPlugin_Axis::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(ConstructionPlugin_Axis::METHOD());
  std::string aMethodType = aMethodTypeAttr->value();
  if (aMethodType == "AxisByPointsCase") {
    createAxisByTwoPoints();
  } else if (aMethodType == "AxisByCylindricalFaceCase") {
    createAxisByCylindricalFace();
  } else if (aMethodType == "AxisByPointAndDirection") {
    createAxisByPointAndDirection();
  }
}

bool ConstructionPlugin_Axis::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
  std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
{
  bool isCustomized = theDefaultPrs.get() != NULL &&
                      theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);

  isCustomized = thePrs->setLineStyle(3) || isCustomized;
  isCustomized = thePrs->setWidth(2) || isCustomized;

  return isCustomized;
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Axis.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Axis.h"

#include <Config_PropManager.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeString.h>

#include <GeomAPI_Edge.h>
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
}

void ConstructionPlugin_Axis::createAxisByTwoPoints()
{
  AttributeSelectionPtr aRef1 = data()->selection(ConstructionPlugin_Axis::POINT_FIRST());
  AttributeSelectionPtr aRef2 = data()->selection(ConstructionPlugin_Axis::POINT_SECOND());
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    GeomShapePtr aShape2 = aRef2->value();
    if (aShape1->isVertex() && aShape2->isVertex() && (!aShape1->isEqual(aShape2))) {
      std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> anEnd = GeomAlgoAPI_PointBuilder::point(aShape2);
      if (aStart->distance(anEnd) > ConstructionPlugin_Axis::MINIMAL_LENGTH()) {
        std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

        ResultConstructionPtr aConstr = document()->createConstruction(data());
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
  }
}

bool ConstructionPlugin_Axis::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
  std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
{
  bool isCustomized = theDefaultPrs.get() != NULL &&
                      theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);

  isCustomized = thePrs->setLineStyle(3);

  return isCustomized;
}

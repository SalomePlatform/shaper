// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Axis.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Axis.h"

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Edge.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

using namespace std;

static const double MINIMAL_LENGTH = 1.e-5;

ConstructionPlugin_Axis::ConstructionPlugin_Axis()
{
}

void ConstructionPlugin_Axis::initAttributes()
{
  data()->addAttribute(POINT_ATTR_FIRST,  ModelAPI_AttributeReference::type());
  data()->addAttribute(POINT_ATTR_SECOND, ModelAPI_AttributeReference::type());
}

void ConstructionPlugin_Axis::execute()
{
  AttributeReferencePtr aRef1 = data()->reference(POINT_ATTR_FIRST);
  AttributeReferencePtr aRef2 = data()->reference(POINT_ATTR_SECOND);
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    ResultConstructionPtr aPntObj1 = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aRef1->value());
    ResultConstructionPtr aPntObj2 = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aRef2->value());
    if ((aPntObj1.get() != NULL) && (aPntObj2.get() != NULL)) {
      GeomShapePtr aShape1 = aPntObj1->shape();
      GeomShapePtr aShape2 = aPntObj2->shape();
      if (aShape1->isVertex() && aShape2->isVertex()) {
        std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
        std::shared_ptr<GeomAPI_Pnt> anEnd = GeomAlgoAPI_PointBuilder::point(aShape2);
        if (aStart->distance(anEnd) > MINIMAL_LENGTH) {
          std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

          ResultConstructionPtr aConstr = document()->createConstruction(data());
          aConstr->setShape(anEdge);
          setResult(aConstr);
        }
      }
    }
  }
}

void ConstructionPlugin_Axis::customisePresentation(AISObjectPtr thePrs)
{
  thePrs->setColor(0, 0, 0);
  thePrs->setLineStyle(3);
}
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCoincidence.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCoincidence.h"

#include <SketchPlugin_Point.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <ModelGeomAlgo_Point2D.h>
#include <GeomDataAPI_Point2D.h>

#include <SketcherPrs_Factory.h>

SketchPlugin_ConstraintCoincidence::SketchPlugin_ConstraintCoincidence()
{
}

void SketchPlugin_ConstraintCoincidence::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintCoincidence::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCoincidence::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::coincidentConstraint(this, sketch()->coordinatePlane(),
                                                                 thePrevious);
  return anAIS;
}

FeaturePtr SketchPlugin_ConstraintCoincidence::findCoincidenceFeature(const FeaturePtr& theFeature1,
                                                                      const FeaturePtr& theFeature2)
{
  FeaturePtr aResultFeature;

  std::list<AttributePtr> anAttrList;
  if (theFeature1->getKind() == SketchPlugin_Circle::ID() ||
      theFeature2->getKind() == SketchPlugin_Circle::ID())
    return aResultFeature;

  if (theFeature2->getKind() == SketchPlugin_Line::ID()) {
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Line::START_ID()));
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Line::END_ID()));
  } else if (theFeature2->getKind() == SketchPlugin_Arc::ID()) {
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Arc::START_ID()));
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Arc::END_ID()));
  }

  const std::set<AttributePtr>& aRefsList = theFeature1->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aRefIt = aRefsList.begin();
  for (; aRefIt != aRefsList.end() && !aResultFeature.get(); ++aRefIt) {
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
    if (aConstrFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
      continue;
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
    AttributePtr anAttr = aRefAttr->attr();
    if (anAttr->id() == SketchPlugin_Arc::CENTER_ID())
      continue;

    anAttr = aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_A());
    if (anAttr == *aRefIt)
      anAttr = aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_B());

    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
    if (!aRefAttr)
      continue;
    anAttr = aRefAttr->attr();
    for (std::list<AttributePtr>::const_iterator anIt = anAttrList.begin();
         anIt != anAttrList.end() && !aResultFeature.get(); ++anIt)
      if (*anIt == anAttr)
        aResultFeature = aConstrFeature;
  }
  return aResultFeature;
}

AttributePoint2DPtr SketchPlugin_ConstraintCoincidence::getPoint(const FeaturePtr& theFeature)
{
  AttributePoint2DPtr aPoint = ModelGeomAlgo_Point2D::getPointOfRefAttr(theFeature.get(),
                                                         SketchPlugin_Constraint::ENTITY_A(),
                                                         SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
  if (!aPoint.get())
    aPoint = ModelGeomAlgo_Point2D::getPointOfRefAttr(theFeature.get(),
                                                      SketchPlugin_Constraint::ENTITY_B(),
                                                      SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
  return aPoint;
}

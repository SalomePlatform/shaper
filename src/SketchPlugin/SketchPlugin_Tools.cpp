// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Tools.cpp
// Created:     07 July 2015
// Author:      Sergey POKHODENKO

#include "SketchPlugin_Tools.h"

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <SketcherPrs_Tools.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_SketchEntity.h>

namespace SketchPlugin_Tools {

void clearExpressions(AttributeDoublePtr theAttribute)
{
  theAttribute->setText(std::string());
}

void clearExpressions(AttributePointPtr theAttribute)
{
  theAttribute->setText(std::string(), std::string(), std::string());
}

void clearExpressions(AttributePoint2DPtr theAttribute)
{
  theAttribute->setText(std::string(), std::string());
}

void clearExpressions(AttributePtr theAttribute)
{
  // Double
  AttributeDoublePtr anAttributeDouble = 
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (anAttributeDouble.get())
    clearExpressions(anAttributeDouble);
  // Point
  AttributePointPtr anAttributePoint = 
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (anAttributePoint.get())
    clearExpressions(anAttributePoint);
  // Point2D
  AttributePoint2DPtr anAttributePoint2D = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (anAttributePoint2D.get())
    clearExpressions(anAttributePoint2D);
}

void clearExpressions(FeaturePtr theFeature)
{
  if (!theFeature.get())
    return;

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());  
  std::list<AttributePtr>::iterator anAttributeIt = anAttributes.begin();
  for (; anAttributeIt != anAttributes.end(); ++anAttributeIt) {
    clearExpressions(*anAttributeIt);
  }
}

std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(const FeaturePtr theStartCoin)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(), 
                                                                    SketchPlugin_Constraint::ENTITY_A());
  if (aPnt.get() == NULL)
    aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(), SketchPlugin_Constraint::ENTITY_B());
  return aPnt;
}

void findCoincidences(const FeaturePtr theStartCoin,
                      const std::string& theAttr,
                      std::set<FeaturePtr>& theList)
{
  AttributeRefAttrPtr aPnt = theStartCoin->refattr(theAttr);
  if(!aPnt) {
    return;
  }
  FeaturePtr aObj = ModelAPI_Feature::feature(aPnt->object());
  if(theList.find(aObj) == theList.end()) {
    std::shared_ptr<GeomAPI_Pnt2d> aOrig = getCoincidencePoint(theStartCoin);
    if(aOrig.get() == NULL) {
      return;
    }
    theList.insert(aObj);
    const std::set<AttributePtr>& aRefsList = aObj->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aIt;
    for(aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if(aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincidencePoint(aConstrFeature);
        if(aPnt.get() && aOrig->isEqual(aPnt)) {
          findCoincidences(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_A(), theList);
          findCoincidences(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_B(), theList);
        }
      }
    }
  }
}

void updateAngleAttribute(const AttributePtr& theFirstAngleAttribute,
                          const AttributePtr& theSecondAngleAttribute,
                          const int& theNumberOfCopies,
                          const bool toFullAngle)
{
  if (theNumberOfCopies == 0)
    return;

  AttributeDoublePtr aDoubleFirstAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
                                                                theFirstAngleAttribute);
  double aValue = aDoubleFirstAttr->value();

  AttributeDoublePtr aDoubleSecondAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
                                                                theSecondAngleAttribute);
  if (toFullAngle)
    aDoubleSecondAttr->setValue(aValue*theNumberOfCopies);
  else
    aDoubleSecondAttr->setValue(aValue/theNumberOfCopies);
}

} // namespace SketchPlugin_Tools

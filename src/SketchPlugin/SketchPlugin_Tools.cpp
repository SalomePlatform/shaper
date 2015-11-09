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

void updateMultiAttribute(const AttributePtr& theFirstAngleAttribute,
                          const AttributePtr& theSecondAngleAttribute,
                          const int& theValue,
                          const bool toMultiply)
{
  if (theValue == 0 || !theFirstAngleAttribute->isInitialized())
    return;

  AttributeDoublePtr aDoubleFirstAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
                                                                theFirstAngleAttribute);
  double aValue = aDoubleFirstAttr->value();

  AttributeDoublePtr aDoubleSecondAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
                                                                theSecondAngleAttribute);
  if (toMultiply)
    aDoubleSecondAttr->setValue(aValue*theValue);
  else
    aDoubleSecondAttr->setValue(aValue/theValue);
}

void updateMultiAttribute(const AttributePtr& theFirstAttribute,
                          const AttributePtr& theSecondAttribute,
                          const AttributePtr& theModifiedAttribute,
                          const int& theValue,
                          const bool toMultiply)
{
  if (theValue == 0 || !theFirstAttribute->isInitialized()
                    || !theSecondAttribute->isInitialized())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aFirstPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFirstAttribute);
  std::shared_ptr<GeomDataAPI_Point2D> aSecondPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theSecondAttribute);
  std::shared_ptr<GeomDataAPI_Point2D> aModifiedPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theModifiedAttribute);

  if (!aFirstPoint.get() || !aSecondPoint.get() || !aModifiedPoint.get())
    return;

  if (aFirstPoint->pnt()->isEqual(aSecondPoint->pnt()))
    aModifiedPoint->setValue(aFirstPoint->pnt());
  else {
    double aDx = aSecondPoint->x() - aFirstPoint->x();
    double aDy = aSecondPoint->y() - aFirstPoint->y();

    double aX  = toMultiply ? aDx * theValue : aDx / theValue;
    double anY = toMultiply ? aDy * theValue : aDy / theValue;

    aModifiedPoint->setValue(aFirstPoint->x() + aX, aFirstPoint->y() + anY);
  }
}

} // namespace SketchPlugin_Tools

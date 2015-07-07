// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Tools.cpp
// Created:     07 July 2015
// Author:      Sergey POKHODENKO

#include "SketchPlugin_Tools.h"

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>

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

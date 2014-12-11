// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.cpp
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_ResultValidators.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>
#include <GeomAPI_Curve.h>

ResultPtr result(const ObjectPtr theObject)
{
  return std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
}

bool SketchPlugin_ResultPointValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  std::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isVertex();
}

bool SketchPlugin_ResultLineValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  std::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isEdge() && GeomAPI_Curve(aShape).isLine();
}

bool SketchPlugin_ResultArcValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  std::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isEdge() && GeomAPI_Curve(aShape).isCircle();
}

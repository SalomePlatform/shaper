// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "ModelAPI_Tools.h"
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>

namespace ModelAPI_Tools {

  std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult)
  {
/*
    ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
    if (aBody)
      return aBody->shape();

    ResultConstructionPtr aConstruct = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
      theResult);
    if (aConstruct)
      return aConstruct->shape();

    ResultGroupPtr aGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(theResult);
    if (aGroup)
      return aGroup->shape();
    return std::shared_ptr<GeomAPI_Shape>();
    */
    return theResult->shape();
  }
}

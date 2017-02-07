// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_EntityWrapper.h>

PlaneGCSSolver_EntityWrapper::PlaneGCSSolver_EntityWrapper(const GCSCurvePtr theEntity)
  : myEntity(theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(myEntity);
  if (aLine) myType = ENTITY_LINE;
  else {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(myEntity);
    if (anArc) myType = ENTITY_ARC;
    else {
      std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(myEntity);
      if (aCircle) myType = ENTITY_CIRCLE;
    }
  }
}

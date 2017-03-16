// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityDestroyer.cpp
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_EntityDestroyer.h>

#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>

static void destroyScalar(const EntityWrapperPtr& theEntity, GCS::SET_pD& theParams)
{
  ScalarWrapperPtr aScalar = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(theEntity);
  theParams.insert(aScalar->scalar());
}

static void destroyPoint(const EntityWrapperPtr& theEntity, GCS::SET_pD& theParams)
{
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
  theParams.insert(aPoint->point()->x);
  theParams.insert(aPoint->point()->y);
}

static void destroyLine(const EntityWrapperPtr& theEntity, GCS::SET_pD& theParams)
{
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(anEntity->entity());
  theParams.insert(aLine->p1.x);
  theParams.insert(aLine->p1.y);
  theParams.insert(aLine->p2.x);
  theParams.insert(aLine->p2.y);
}

static void destroyCircle(const EntityWrapperPtr& theEntity, GCS::SET_pD& theParams)
{
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Circle> aCirc = std::dynamic_pointer_cast<GCS::Circle>(anEntity->entity());
  theParams.insert(aCirc->center.x);
  theParams.insert(aCirc->center.y);
  theParams.insert(aCirc->rad);
}

static void destroyArc(const EntityWrapperPtr& theEntity, GCS::SET_pD& theParams)
{
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);
  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());
  theParams.insert(anArc->center.x);
  theParams.insert(anArc->center.y);
  theParams.insert(anArc->start.x);
  theParams.insert(anArc->start.y);
  theParams.insert(anArc->end.x);
  theParams.insert(anArc->end.y);
  theParams.insert(anArc->startAngle);
  theParams.insert(anArc->endAngle);
  theParams.insert(anArc->rad);
}

void PlaneGCSSolver_EntityDestroyer::remove(const EntityWrapperPtr& theEntity)
{
  GCS::SET_pD& aParamSet = theEntity->isExternal() ? myParamsOutOfStorage : myParams;

  switch (theEntity->type()) {
  case ENTITY_SCALAR:
  case ENTITY_ANGLE:
    destroyScalar(theEntity, aParamSet);
    break;
  case ENTITY_POINT:
    destroyPoint(theEntity, aParamSet);
    break;
  case ENTITY_LINE:
    destroyLine(theEntity, aParamSet);
    break;
  case ENTITY_CIRCLE:
    destroyCircle(theEntity, aParamSet);
    break;
  case ENTITY_ARC:
    destroyArc(theEntity, aParamSet);
    break;
  default: break;
  }
}

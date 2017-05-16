// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityDestroyer.cpp
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_EntityDestroyer.h>

#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_Tools.h>

void PlaneGCSSolver_EntityDestroyer::remove(const EntityWrapperPtr& theEntity)
{
  GCS::SET_pD aParameters = PlaneGCSSolver_Tools::parameters(theEntity);
  if (theEntity->isExternal())
    myParamsOutOfStorage.insert(aParameters.begin(), aParameters.end());
  else
    myParams.insert(aParameters.begin(), aParameters.end());
}

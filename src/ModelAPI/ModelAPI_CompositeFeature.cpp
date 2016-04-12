// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_CompositeFeature.cpp
// Created:     20 Oct 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_CompositeFeature.h>

ModelAPI_CompositeFeature::~ModelAPI_CompositeFeature()
{

}

void ModelAPI_CompositeFeature::exchangeIDs(
  std::shared_ptr<ModelAPI_Feature> theFeature1, std::shared_ptr<ModelAPI_Feature> theFeature2)
{
  // by default nothing is in the implementation
}

void ModelAPI_CompositeFeature::erase()
{
  // erase all sub-features
  if (data()->isValid()) {
    for(int a = numberOfSubs(); a > 0; a--) {
      FeaturePtr aFeature = subFeature(a - 1);
      if (aFeature.get()) {
        // subs are referenced from sketch, but must be removed for sure, so not checkings
        aFeature->document()->removeFeature(aFeature);
      }
    }
  }
  ModelAPI_Feature::erase();
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetConstraintShapeSelector.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <PartSet_Tools.h>
#include <SketchPlugin_Feature.h>


//*********************************************
bool PartSet_WidgetConstraintShapeSelector::storeValueCustom() const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(mySelectedObject);
  if (aFeature) {
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if ((!aSPFeature) && (!myShape->isNull())) {
      ObjectPtr aObj = PartSet_Tools::createFixedObjectByExternal(myShape->impl<TopoDS_Shape>(),
                                                                  mySelectedObject, mySketch);
      if (aObj) {
        PartSet_WidgetConstraintShapeSelector* that = (PartSet_WidgetConstraintShapeSelector*) this;
        that->mySelectedObject = aObj;
      } else 
        return false;
    }
  }
  return ModuleBase_WidgetShapeSelector::storeValueCustom();
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetConstraintShapeSelector.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <PartSet_Tools.h>
#include <SketchPlugin_Feature.h>

bool PartSet_WidgetConstraintShapeSelector::setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape)
{
  // initially the method is wrote to create an external object. Since the parent widget creates it,
  // the redefinition is not necessary anymore.
  // TODO: remove the widget and use the parent one insted of it
  return PartSet_WidgetShapeSelector::setObject(theSelectedObject, theShape);
  /*ObjectPtr aSelectedObject = theSelectedObject;

  FeaturePtr aFeature = ModelAPI_Feature::feature(aSelectedObject);
  if (aFeature) {
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if ((!aSPFeature) && (!theShape->isNull())) {
      createExternal(theSelectedObject, theShape);

      if (myExternalObject) {
        aSelectedObject = myExternalObject;
      } else 
        return false;
    }
  }
  return ModuleBase_WidgetShapeSelector::setObject(aSelectedObject, theShape);*/
}

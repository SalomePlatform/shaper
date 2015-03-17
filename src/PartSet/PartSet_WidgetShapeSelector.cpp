// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetShapeSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_RefAttrValidator.h>
#include <ModelAPI_ResultValidator.h>

#include <PartSet_Tools.h>
#include <SketchPlugin_Feature.h>


bool PartSet_WidgetShapeSelector::storeValueCustom() const
{
  if (!mySelectedObject)
    return false;

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(mySelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  if ((!aSPFeature) && (!myShape->isNull())) {
    // Processing of external (non-sketch) object
    ObjectPtr aObj = PartSet_Tools::createFixedObjectByExternal(myShape->impl<TopoDS_Shape>(),
                                                                mySelectedObject, mySketch);
    if (aObj) {
      PartSet_WidgetShapeSelector* that = (PartSet_WidgetShapeSelector*) this;
      that->mySelectedObject = aObj;
    } else 
      return false;
  } else {
    // Processing of sketch object
    DataPtr aData = myFeature->data();
    if (myShape) {
      AttributePtr aAttr = aData->attribute(attributeID());
      AttributeRefAttrPtr aRefAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
      if (aRefAttr) {
        TopoDS_Shape aShape = myShape->impl<TopoDS_Shape>();
        AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(mySelectedObject, aShape, mySketch);

        // this is an alternative, whether the attribute should be set or object in the attribute
        // the first check is the attribute because the object already exist
        // the object is set only if there is no selected attribute
        // test case is - preselection for distance operation, which contains two points selected on lines
        if (aPntAttr)
          aRefAttr->setAttr(aPntAttr);
        else if (mySelectedObject)
          aRefAttr->setObject(mySelectedObject);
        updateObject(myFeature);
        return true;
      }
    }
  }
  return ModuleBase_WidgetShapeSelector::storeValueCustom();
}


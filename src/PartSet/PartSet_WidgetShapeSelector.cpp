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

//********************************************************************
bool PartSet_WidgetShapeSelector::isValid(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape)
{
  // the method is redefined to analize the selected shape in validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> aValidators;
  std::list<std::list<std::string> > anArguments;
  aFactory->validators(parentID(), attributeID(), aValidators, anArguments);

  // Check the type of selected object
  std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
  bool isValid = true;
  for (; aValidator != aValidators.end(); aValidator++) {
    const ModelAPI_ResultValidator* aResValidator =
        dynamic_cast<const ModelAPI_ResultValidator*>(*aValidator);
    if (aResValidator) {
      isValid = false;
      if (aResValidator->isValid(theObj)) {
        isValid = true;
        break;
      }
    }
  }
  if (!isValid)
    return false;

  // Check the acceptability of the object and shape as validator attribute
  AttributePtr aPntAttr;
  DataPtr aData = myFeature->data();
  if (theShape.get() != NULL) {
    AttributePtr aAttr = aData->attribute(attributeID());
    AttributeRefAttrPtr aRefAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
    if (aRefAttr) {
      TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
      aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theObj, aShape, mySketch);
    }
  }
  // Check the acceptability of the object as attribute
  aValidator = aValidators.begin();
  std::list<std::list<std::string> >::iterator aArgs = anArguments.begin();
  for (; aValidator != aValidators.end(); aValidator++, aArgs++) {
    const ModelAPI_RefAttrValidator* aAttrValidator =
        dynamic_cast<const ModelAPI_RefAttrValidator*>(*aValidator);
    if (aAttrValidator) {
      if (aPntAttr.get() != NULL)
      {
        if (!aAttrValidator->isValid(myFeature, *aArgs, aPntAttr)) {
          return false;
        }
      }
      else
      {
        if (!aAttrValidator->isValid(myFeature, *aArgs, theObj)) {
          return false;
        }
      }
    }
  }
  return true;
}

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

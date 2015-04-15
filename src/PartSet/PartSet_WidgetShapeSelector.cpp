// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetShapeSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <Config_WidgetAPI.h>

#include <PartSet_Tools.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

PartSet_WidgetShapeSelector::PartSet_WidgetShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId), myUseExternal(true)
{
  QString aIsExternal(theData->getProperty("use_external").c_str());
  if (!aIsExternal.isEmpty()) {
    QString aStr = aIsExternal.toUpper();
    myUseExternal = (aStr == "TRUE") || (aStr == "YES"); 
  }
}

bool PartSet_WidgetShapeSelector::setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape)
{
  ObjectPtr aSelectedObject = theSelectedObject;
  GeomShapePtr aShape = theShape;

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aSelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;
  // Do check using of external feature
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);

  // Do check that we can use external feature
  if ((aSPFeature.get() != NULL) && aSPFeature->isExternal() && (!myUseExternal))
    return false;

  if (aSPFeature.get() == NULL && aShape.get() != NULL && !aShape->isNull() && myUseExternal) {
    aSelectedObject = PartSet_Tools::findFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                  theSelectedObject, mySketch);
    if (!aSelectedObject.get()) {
      // Processing of external (non-sketch) object
      aSelectedObject = PartSet_Tools::createFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                  theSelectedObject, mySketch);
      if (aSelectedObject.get())
        myExternalObject = aSelectedObject;
    }
  } else {
    // Processing of sketch object
    DataPtr aData = myFeature->data();
    if (aShape) {
      AttributePtr aAttr = aData->attribute(attributeID());
      AttributeRefAttrPtr aRefAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
      if (aRefAttr) {
        // it is possible that the point feature is selected. It should be used itself
        // instead of searching an attribute for the shape
        bool aShapeIsResult = false;
        /*ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
        if (aResult.get() != NULL) {
          GeomShapePtr aShapePtr = aResult->shape();
          // it is important to call isEqual of the shape of result.
          // It is a GeomAPI_Vertex shape for the point. The shape of the parameter is 
          // GeomAPI_Shape. It is important to use the realization of the isEqual method from
          // GeomAPI_Vertex class
          aShapeIsResult = aShapePtr.get() != NULL && aShapePtr->isEqual(theShape);
        }*/

        AttributePtr aPntAttr;
        if (!aShapeIsResult) {
          TopoDS_Shape aTDSShape = aShape->impl<TopoDS_Shape>();
          aPntAttr = PartSet_Tools::findAttributeBy2dPoint(aSelectedObject, aTDSShape, mySketch);
        }
        // this is an alternative, whether the attribute should be set or object in the attribute
        // the first check is the attribute because the object already exist
        // the object is set only if there is no selected attribute
        // test case is - preselection for distance operation, which contains two points selected on lines
        if (aPntAttr)
          aRefAttr->setAttr(aPntAttr);
        else if (aSelectedObject)
          aRefAttr->setObject(aSelectedObject);
        return true;
      }
    }
  }
  return ModuleBase_WidgetShapeSelector::setObject(aSelectedObject, aShape);
}

//********************************************************************
void PartSet_WidgetShapeSelector::restoreAttributeValue(const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theValid);
  removeExternal();
}

//********************************************************************
void PartSet_WidgetShapeSelector::removeExternal()
{
  if (myExternalObject.get()) {
    DocumentPtr aDoc = myExternalObject->document();
    FeaturePtr aFeature = ModelAPI_Feature::feature(myExternalObject);
    if (aFeature.get() != NULL) {
      QObjectPtrList anObjects;
      anObjects.append(aFeature);
      // the external feature should be removed with all references, sketch feature should be ignored
      std::set<FeaturePtr> anIgnoredFeatures;
      anIgnoredFeatures.insert(sketch());
      // the current feature should be ignored, because it can use the external feature in the
      // attributes and, therefore have a references to it. So, the delete functionality tries
      // to delete this feature. Test case is creation of a constraint on external point,
      // use in this control after an external point, the point of the sketch.
      anIgnoredFeatures.insert(myFeature);
      XGUI_Workshop::deleteFeatures(anObjects, anIgnoredFeatures);
    }
    myExternalObject = ObjectPtr();
  }
}

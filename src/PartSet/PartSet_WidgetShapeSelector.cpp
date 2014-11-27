// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetShapeSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <PartSet_Tools.h>


bool PartSet_WidgetShapeSelector::storeValue() const
{
  if (!mySelectedObject)
    return false;

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(mySelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;

  DataPtr aData = myFeature->data();
  if (myUseSubShapes && myShape) {
    AttributePtr aAttr = aData->attribute(attributeID());
    AttributeRefAttrPtr aRefAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
    if (aRefAttr) {
      TopoDS_Shape aShape = myShape->impl<TopoDS_Shape>();
      AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(mySelectedObject, aShape, mySketch);
      if (mySelectedObject)
        aRefAttr->setObject(mySelectedObject);
      if (aPntAttr)
        aRefAttr->setAttr(aPntAttr);

      updateObject(myFeature);
      return true;
    }
  }
  return ModuleBase_WidgetShapeSelector::storeValue();
}

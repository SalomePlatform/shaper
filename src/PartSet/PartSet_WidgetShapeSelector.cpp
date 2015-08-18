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
#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <StdSelect_BRepOwner.hxx>

PartSet_WidgetShapeSelector::PartSet_WidgetShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId)
{
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), true);
}

PartSet_WidgetShapeSelector::~PartSet_WidgetShapeSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
bool PartSet_WidgetShapeSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  bool aValid = ModuleBase_WidgetShapeSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

//********************************************************************
void PartSet_WidgetShapeSelector::setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape)
{
  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theSelectedObject);
  // Do check using of external feature
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // Processing of sketch object
  if (aSPFeature.get() != NULL) {
    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
      if (aResult.get()) {
        aShape = aResult->shape();
      }
    }
    setPointAttribute(theSelectedObject, aShape);
  }
  else
    ModuleBase_WidgetShapeSelector::setObject(theSelectedObject, theShape);
}

void PartSet_WidgetShapeSelector::getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetShapeSelector::getGeomSelection(thePrs, theObject, theShape);

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists, try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetShapeSelector
  if (aSPFeature.get() == NULL && myExternalObjectMgr->useExternal()) {
    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
      if (aResult.get())
        aShape = aResult->shape();
    }
    if (aShape.get() != NULL && !aShape->isNull())
      theObject = myExternalObjectMgr->externalObject(theObject, aShape, sketch());
  }
}

//********************************************************************
GeomShapePtr PartSet_WidgetShapeSelector::getShape() const
{
  // an empty shape by default
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  GeomShapePtr aShape = PartSet_Tools::findShapeBy2DPoint(anAttribute, myWorkshop);

  if (!aShape.get())
    aShape = ModuleBase_WidgetShapeSelector::getShape();
  return aShape;
}

//********************************************************************
void PartSet_WidgetShapeSelector::restoreAttributeValue(const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop);
}

//********************************************************************
void PartSet_WidgetShapeSelector::setPointAttribute(ObjectPtr theSelectedObject, GeomShapePtr theShape)
{
  DataPtr aData = myFeature->data();
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
      TopoDS_Shape aTDSShape = theShape->impl<TopoDS_Shape>();
      aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theSelectedObject, aTDSShape, mySketch);
    }
    // this is an alternative, whether the attribute should be set or object in the attribute
    // the first check is the attribute because the object already exist
    // the object is set only if there is no selected attribute
    // test case is - preselection for distance operation, which contains two points selected on lines
    if (aPntAttr)
      aRefAttr->setAttr(aPntAttr);
    else
      aRefAttr->setObject(theSelectedObject);
  }
}

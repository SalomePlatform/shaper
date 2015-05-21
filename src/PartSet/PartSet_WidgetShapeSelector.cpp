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

bool PartSet_WidgetShapeSelector::setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape)
{
  ObjectPtr aSelectedObject = theSelectedObject;
  //GeomShapePtr aShape = theShape;

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aSelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;
  // Do check using of external feature
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);

  // Do check that we can use external feature
  if ((aSPFeature.get() != NULL) && aSPFeature->isExternal() && (!myExternalObjectMgr->useExternal()))
    return false;

  if (aSPFeature.get() == NULL && theShape.get() != NULL && !theShape->isNull() && myExternalObjectMgr->useExternal()) {
    aSelectedObject = myExternalObjectMgr->externalObject(theSelectedObject, theShape, sketch());
  } else {
    // Processing of sketch object
    if (theShape.get()) {
      setPointAttribute(theSelectedObject, theShape);
      return true;
    }
  }
  return ModuleBase_WidgetShapeSelector::setObject(aSelectedObject, theShape);
}

//********************************************************************
GeomShapePtr PartSet_WidgetShapeSelector::getShape() const
{
  // an empty shape by default
  GeomShapePtr aShape;

  // 1. find an attribute value in attribute reference attribute value
  DataPtr aData = myFeature->data();
  AttributePtr aAttr = aData->attribute(attributeID());
  AttributeRefAttrPtr aRefAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
  if (aRefAttr) {
    if (!aRefAttr->isObject()) {
      AttributePtr anAttribute = aRefAttr->attr();
      if (anAttribute.get()) {
        XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
        XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

        // 2. find visualized vertices of the attribute and if the attribute of the vertex is
        // the same, return it
        FeaturePtr anAttributeFeature = ModelAPI_Feature::feature(anAttribute->owner());
        // 2.1 get visualized results of the feature
        const std::list<ResultPtr>& aResList = anAttributeFeature->results();
        std::list<ResultPtr>::const_iterator anIt = aResList.begin(), aLast = aResList.end();
        for (; anIt != aLast; anIt++) {
          AISObjectPtr aAISObj = aDisplayer->getAISObject(*anIt);
          if (aAISObj.get() != NULL) {
            Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
            // 2.2 find selected owners of a visualizedd object
            SelectMgr_IndexedMapOfOwner aSelectedOwners;  
            aConnector->workshop()->selector()->selection()->entityOwners(anAISIO, aSelectedOwners);
            for  (Standard_Integer i = 1, n = aSelectedOwners.Extent(); i <= n; i++) {
              Handle(SelectMgr_EntityOwner) anOwner = aSelectedOwners(i);
              if (!anOwner.IsNull()) {
                Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(anOwner);
                if (!aBRepOwner.IsNull() && aBRepOwner->HasShape()) {
                  const TopoDS_Shape& aBRepShape = aBRepOwner->Shape();
                  if (aBRepShape.ShapeType() == TopAbs_VERTEX) {
                    // 2.3 if the owner is vertex and an attribute of the vertex is equal to the initial
                    // attribute, returns the shape
                    AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(anAttributeFeature,
                                                                                  aBRepShape, sketch());
                    if (aPntAttr.get() != NULL && aPntAttr == anAttribute) {
                      aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
                      aShape->setImpl(new TopoDS_Shape(aBRepShape));
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if (!aShape.get())
    aShape = ModuleBase_WidgetShapeSelector::getShape();
  return aShape;
}

//********************************************************************
void PartSet_WidgetShapeSelector::restoreAttributeValue(const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature);
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

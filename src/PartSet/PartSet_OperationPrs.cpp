// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.cpp
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_OperationPrs.h"
#include "PartSet_Tools.h"

#include "XGUI_Workshop.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Displayer.h"

#include "ModuleBase_Tools.h"
#include "ModuleBase_IModule.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultCompSolid.h>

#include <GeomValidators_Tools.h>

#include <GeomAPI_IPresentable.h>

#include <StdPrs_WFDeflectionShape.hxx>

#include <QList>

IMPLEMENT_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_OperationPrs, ViewerData_AISShape);

PartSet_OperationPrs::PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop)
  : ViewerData_AISShape(TopoDS_Shape()), myFeature(FeaturePtr()), myWorkshop(theWorkshop)
{
  myShapeColor = ModuleBase_Tools::color("Visualization", "construction_plane_color", "1,1,0");
  myResultColor = ModuleBase_Tools::color("Visualization", "construction_plane_color", "0,1,0");
}

bool PartSet_OperationPrs::canActivate(const FeaturePtr& theFeature)
{
  bool aHasSelectionAttribute = false;

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes("");
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  for (; anIt != aLast && !aHasSelectionAttribute; anIt++)
    aHasSelectionAttribute = isSelectionAttribute(*anIt);

  return aHasSelectionAttribute;
}

void PartSet_OperationPrs::setFeature(const FeaturePtr& theFeature)
{
  myFeature = theFeature;
  updateShapes();
}

/*bool PartSet_OperationPrs::dependOn(const ObjectPtr& theResult)
{
  return myFeatureShapes.contains(theResult);
}*/

void PartSet_OperationPrs::updateShapes()
{
  myFeatureShapes.clear();
  getFeatureShapes(myFeatureShapes);

  myFeatureResults.clear();
  if (myFeature)
    myFeatureResults = myFeature->results();
}

void PartSet_OperationPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  // when the feature can not be visualized in the module, the operation preview should not
  // be visualized also
  if (!myWorkshop->module()->canDisplayObject(myFeature))
    return;

  Quantity_Color aColor(1., 1., 0., Quantity_TOC_RGB); // yellow
  SetColor(aColor);

  thePresentation->Clear();
  XGUI_Displayer* aDisplayer = workshop()->displayer();

  // create presentations on the base of the shapes
  Handle(Prs3d_Drawer) aDrawer = Attributes();
  QMap<ObjectPtr, QList<GeomShapePtr> >::const_iterator anIt = myFeatureShapes.begin(),
                                                        aLast = myFeatureShapes.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = anIt.key();
    if (!isVisible(aDisplayer, anObject))
      continue;
    QList<GeomShapePtr> aShapes = anIt.value();
    QList<GeomShapePtr>::const_iterator aShIt = aShapes.begin(), aShLast = aShapes.end();
    for (; aShIt != aShLast; aShIt++) {
      GeomShapePtr aGeomShape = *aShIt;
      if (!aGeomShape.get())
        continue;
      TopoDS_Shape aShape = aGeomShape->impl<TopoDS_Shape>();
      // change deviation coefficient to provide more precise circle
      ModuleBase_Tools::setDefaultDeviationCoefficient(aShape, aDrawer);
      StdPrs_WFDeflectionShape::Add(thePresentation, aShape, aDrawer);
    }
  }

  aColor = Quantity_Color(0., 1., 0., Quantity_TOC_RGB); // green
  SetColor(aColor);

  std::list<ResultPtr>::const_iterator aRIt = myFeatureResults.begin(),
                                       aRLast = myFeatureResults.end();
  for (; aRIt != aRLast; aRIt++) {
    ResultPtr aResult = *aRIt;
    if (!isVisible(aDisplayer, aResult))
      continue;
    GeomShapePtr aGeomShape = aResult->shape();
    if (!aGeomShape.get())
      continue;
    TopoDS_Shape aShape = aGeomShape->impl<TopoDS_Shape>();
    // change deviation coefficient to provide more precise circle
    ModuleBase_Tools::setDefaultDeviationCoefficient(aShape, aDrawer);
    StdPrs_WFDeflectionShape::Add(thePresentation, aShape, aDrawer);
  }
}

void PartSet_OperationPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  // the presentation is not used in the selection
}

bool PartSet_OperationPrs::isVisible(XGUI_Displayer* theDisplayer, const ObjectPtr& theObject)
{
  bool aVisible = false;
  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aPrs.get() || aResult.get())
    aVisible = theDisplayer->isVisible(theObject);
  else {
    // check if all results of the feature are visible
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    aVisible = !aResults.empty();
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aVisible = aVisible && theDisplayer->isVisible(*aIt);
    }
  }
  return aVisible;
}

bool isSubObject(const ObjectPtr& theObject, const FeaturePtr& theFeature)
{
  bool isSub = false;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aComposite.get())
    isSub = aComposite->isSub(theObject);

  return isSub;
}

void addValue(const ObjectPtr& theObject, const GeomShapePtr& theShape,
              const FeaturePtr& theFeature,
              QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  if (theObject.get()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theObject);
      if (aCompsolidResult.get()) {
        for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
          ResultPtr aSubResult = aCompsolidResult->subResult(i);
          if (aSubResult.get()) {
            GeomShapePtr aShape;
            addValue(aSubResult, aShape, theFeature, theObjectShapes);
          }
        }
        return;
      }
    }


    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult.get())
        aShape = aResult->shape();
    }
    if (!isSubObject(theObject, theFeature)) {
      if (theObjectShapes.contains(theObject))
        theObjectShapes[theObject].append(aShape);
      else {
        QList<GeomShapePtr> aShapes;
        aShapes.append(aShape);
        theObjectShapes[theObject] = aShapes;
      }
    }
  }
}

void PartSet_OperationPrs::getFeatureShapes(QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  if (!myFeature.get())
    return;

  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  QList<GeomShapePtr> aShapes;
  std::list<AttributePtr> anAttributes = myFeature->data()->attributes("");
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    if (!isSelectionAttribute(anAttribute))
      continue;

    if (!aValidators->isCase(myFeature, anAttribute->id()))
      continue; // this attribute is not participated in the current case

    std::string anAttrType = anAttribute->attributeType();

    if (anAttrType == ModelAPI_AttributeSelectionList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList = 
              std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
      for(int i = 0; i < aCurSelList->size(); i++) {
        std::shared_ptr<ModelAPI_AttributeSelection> aSelAttribute = aCurSelList->value(i);
        ResultPtr aResult = aSelAttribute->context();
        GeomShapePtr aShape = aSelAttribute->value();
        addValue(aResult, aShape, myFeature, theObjectShapes);
      }
    }
    if (anAttrType == ModelAPI_AttributeRefList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttribute);
      for (int i = 0; i < aCurSelList->size(); i++) {
        GeomShapePtr aShape;
        addValue(aCurSelList->object(i), aShape, myFeature, theObjectShapes);
      }
    }
    else {
      ObjectPtr anObject;
      GeomShapePtr aShape;
      if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
        AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
        if (anAttr->isObject()) {
          anObject = anAttr->object();
        }
        else {
          aShape = PartSet_Tools::findShapeBy2DPoint(anAttr, myWorkshop);
          // the distance point is not found if the point is selected in the 2nd time
          // TODO: after debug, this check can be removed
          if (!aShape.get())
            continue;
          anObject = anAttr->attr()->owner();
        }
      }
      if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
        AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
        anObject = anAttr->context();
        aShape = anAttr->value();
      }
      if (anAttrType == ModelAPI_AttributeReference::typeId()) {
        AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(anAttribute);
        anObject = anAttr->value();
      }
      addValue(anObject, aShape, myFeature, theObjectShapes);
    }
  }
}

bool PartSet_OperationPrs::isSelectionAttribute(const AttributePtr& theAttribute)
{
  std::string anAttrType = theAttribute->attributeType();

  return anAttrType == ModelAPI_AttributeSelectionList::typeId() ||
         anAttrType == ModelAPI_AttributeRefList::typeId() ||
         anAttrType == ModelAPI_AttributeRefAttr::typeId() ||
         anAttrType == ModelAPI_AttributeSelection::typeId() ||
         anAttrType == ModelAPI_AttributeReference::typeId();
}

XGUI_Workshop* PartSet_OperationPrs::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

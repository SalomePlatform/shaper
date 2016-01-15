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
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ModelWidget.h>

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

#include <GeomAPI_IPresentable.h>

#include <StdPrs_WFDeflectionShape.hxx>

#include <QList>

static const int AIS_DEFAULT_WIDTH = 2;

IMPLEMENT_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_OperationPrs, ViewerData_AISShape);

PartSet_OperationPrs::PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop)
: ViewerData_AISShape(TopoDS_Shape()), myWorkshop(theWorkshop), myUseAISWidth(false)
{
  myShapeColor = Quantity_Color(1, 1, 1, Quantity_TOC_RGB);
}

bool PartSet_OperationPrs::hasShapes()
{
  return !myFeatureShapes.empty();
}

void PartSet_OperationPrs::setShapeColor(const Quantity_Color& theColor)
{
  myShapeColor = theColor;
}

void PartSet_OperationPrs::useAISWidth()
{
  myUseAISWidth = true;
}

void PartSet_OperationPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  SetColor(myShapeColor);
  thePresentation->Clear();

  XGUI_Displayer* aDisplayer = workshop(myWorkshop)->displayer();
  Handle(Prs3d_Drawer) aDrawer = Attributes();

  // create presentations on the base of the shapes
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

      if (myUseAISWidth) {
        AISObjectPtr anAISPtr = aDisplayer->getAISObject(anObject);
        if (anAISPtr.get()) {
          Handle(AIS_InteractiveObject) anIO = anAISPtr->impl<Handle(AIS_InteractiveObject)>();
          if (!anIO.IsNull()) {
            int aWidth = anIO->Width();
            /// workaround for zero width. Else, there will be a crash
            if (aWidth == 0) { // width returns of TSolid shape is zero
              bool isDisplayed = !anIO->GetContext().IsNull();
              aWidth = AIS_DEFAULT_WIDTH;// default width value
            }
            setWidth(aDrawer, aWidth);
          }
        }
      }
      StdPrs_WFDeflectionShape::Add(thePresentation, aShape, aDrawer);
    }
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

void PartSet_OperationPrs::getFeatureShapes(const FeaturePtr& theFeature,
                                            ModuleBase_IWorkshop* theWorkshop,
                                            QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  theObjectShapes.clear();
  if (!theFeature.get())
    return;

  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  QList<GeomShapePtr> aShapes;
  std::list<AttributePtr> anAttributes = theFeature->data()->attributes("");
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    if (!isSelectionAttribute(anAttribute))
      continue;

    if (!aValidators->isCase(theFeature, anAttribute->id()))
      continue; // this attribute is not participated in the current case

    std::string anAttrType = anAttribute->attributeType();

    if (anAttrType == ModelAPI_AttributeSelectionList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList = 
              std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
      for(int i = 0; i < aCurSelList->size(); i++) {
        std::shared_ptr<ModelAPI_AttributeSelection> aSelAttribute = aCurSelList->value(i);
        ResultPtr aResult = aSelAttribute->context();
        GeomShapePtr aShape = aSelAttribute->value();
        addValue(aResult, aShape, theFeature, theObjectShapes);
      }
    }
    if (anAttrType == ModelAPI_AttributeRefList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttribute);
      for (int i = 0; i < aCurSelList->size(); i++) {
        GeomShapePtr aShape;
        addValue(aCurSelList->object(i), aShape, theFeature, theObjectShapes);
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
          aShape = PartSet_Tools::findShapeBy2DPoint(anAttr, theWorkshop);
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
      addValue(anObject, aShape, theFeature, theObjectShapes);
    }
  }
}

void PartSet_OperationPrs::getResultShapes(const FeaturePtr& theFeature,
                                           ModuleBase_IWorkshop* theWorkshop,
                                           QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  theObjectShapes.clear();

  if (!theFeature.get())
    return;

  XGUI_Displayer* aDisplayer = workshop(theWorkshop)->displayer();

  std::list<ResultPtr> aFeatureResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aRIt = aFeatureResults.begin(),
                                       aRLast = aFeatureResults.end();
  for (; aRIt != aRLast; aRIt++) {
    ResultPtr aResult = *aRIt;
    if (!isVisible(aDisplayer, aResult))
      continue;
    GeomShapePtr aGeomShape = aResult->shape();
    if (!aGeomShape.get())
      continue;

    if (theObjectShapes.contains(aResult))
      theObjectShapes[aResult].append(aGeomShape);
    else {
      QList<GeomShapePtr> aShapes;
      aShapes.append(aGeomShape);
      theObjectShapes[aResult] = aShapes;
    }
  }
}

void PartSet_OperationPrs::getHighlightedShapes(ModuleBase_IWorkshop* theWorkshop,
                                                QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  theObjectShapes.clear();

  QList<ModuleBase_ViewerPrs> aValues;
  ModuleBase_IPropertyPanel* aPanel = theWorkshop->propertyPanel();
  if (aPanel) {
    ModuleBase_ModelWidget* aWidget = aPanel->activeWidget();
    if (aWidget) {
      aWidget->getHighlighted(aValues);
    }
  }

  QList<GeomShapePtr> aShapes;
  QList<ModuleBase_ViewerPrs>::const_iterator anIIt = aValues.begin(),
                                              aILast = aValues.end();
  for (; anIIt != aILast; anIIt++) {
    ModuleBase_ViewerPrs aPrs = *anIIt;
    ObjectPtr anObject = aPrs.object();

    GeomShapePtr aGeomShape;

    TopoDS_Shape aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      aGeomShape = GeomShapePtr(new GeomAPI_Shape());
      aGeomShape->setImpl(new TopoDS_Shape(aShape));
    }
    else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get()) {
        aGeomShape = aResult->shape();
      }
    }

    if (theObjectShapes.contains(anObject))
      theObjectShapes[anObject].append(aGeomShape);
    else {
      QList<GeomShapePtr> aShapes;
      aShapes.append(aGeomShape);
      theObjectShapes[anObject] = aShapes;
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

XGUI_Workshop* PartSet_OperationPrs::workshop(ModuleBase_IWorkshop* theWorkshop)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  return aConnector->workshop();
}

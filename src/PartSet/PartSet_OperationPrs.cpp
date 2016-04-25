// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.cpp
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_OperationPrs.h"
#include "PartSet_Tools.h"

#include "XGUI_Workshop.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Displayer.h"
#include "XGUI_Tools.h"

#include "ModuleBase_Tools.h"
#include "ModuleBase_IModule.h"
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Events.h>
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

#include <Events_Error.h>
#include <Events_Loop.h>

#include <GeomAPI_IPresentable.h>

#include <StdPrs_WFDeflectionShape.hxx>

#include <QList>

#include <gp_Pnt.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

//#define DEBUG_EMPTY_SHAPE

// multi-rotation/translation operation
//#define DEBUG_HIDE_COPY_ATTRIBUTE
#ifdef DEBUG_HIDE_COPY_ATTRIBUTE
#include <ModelAPI_AttributeBoolean.h>
#include <SketchPlugin_SketchEntity.h>
#endif

IMPLEMENT_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_OperationPrs, ViewerData_AISShape);

PartSet_OperationPrs::PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop)
: ViewerData_AISShape(TopoDS_Shape()), myWorkshop(theWorkshop), myUseAISWidth(false)
{
  myShapeColor = Quantity_Color(1, 1, 1, Quantity_TOC_RGB);

  // first presentation for having correct Compute until presentation with shapes are set
  gp_Pnt aPnt(0.0, 0.0, 0.0);
  BRepBuilderAPI_MakeVertex aMaker(aPnt);
  TopoDS_Vertex aVertex = aMaker.Vertex();
  myShapeToPrsMap.Bind(aVertex, NULL);
}

bool PartSet_OperationPrs::hasShapes()
{
  return !myShapeToPrsMap.IsEmpty();
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
  bool aReadyToDisplay = !myShapeToPrsMap.IsEmpty();

  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(myWorkshop)->displayer();
  Handle(Prs3d_Drawer) aDrawer = Attributes();
  // create presentations on the base of the shapes
  for(NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)>::Iterator anIter(myShapeToPrsMap);
      anIter.More(); anIter.Next()) {
    const TopoDS_Shape& aShape = anIter.Key();
    // change deviation coefficient to provide more precise circle
    ModuleBase_Tools::setDefaultDeviationCoefficient(aShape, aDrawer);

    if (myUseAISWidth) {
      Handle(AIS_InteractiveObject) anIO = anIter.Value();
      if (!anIO.IsNull()) {
        int aWidth = anIO->Width();
        /// workaround for zero width. Else, there will be a crash
        if (aWidth == 0) { // width returns of TSolid shape is zero
          bool isDisplayed = !anIO->GetContext().IsNull();
          aWidth = PartSet_Tools::getAISDefaultWidth();// default width value
        }
        setWidth(aDrawer, aWidth);
      }
    }
    StdPrs_WFDeflectionShape::Add(thePresentation, aShape, aDrawer);
  }

  if (!aReadyToDisplay) {
    Events_Error::throwException("An empty AIS presentation: PartSet_OperationPrs");
    std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_EMPTY_OPERATION_PRESENTATION)));
    Events_Loop::loop()->send(aMsg);
  }
}

void PartSet_OperationPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  // the presentation is not used in the selection
}

NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)>& PartSet_OperationPrs::shapesMap()
{
  return myShapeToPrsMap;
}

bool isSubObject(const ObjectPtr& theObject, const FeaturePtr& theFeature)
{
  bool isSub = false;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aComposite.get())
    isSub = aComposite->isSub(theObject);

  return isSub;
}

void PartSet_OperationPrs::addValue(const ObjectPtr& theObject, const GeomShapePtr& theShape,
                                    const FeaturePtr& theFeature, ModuleBase_IWorkshop* theWorkshop,
                                    QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  if (theObject.get()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theObject);
      if (aCompsolidResult.get()) {
        if (aCompsolidResult->numberOfSubs() > 0) {
          for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
            ResultPtr aSubResult = aCompsolidResult->subResult(i);
            if (aSubResult.get()) {
              GeomShapePtr aShape;
              addValue(aSubResult, aShape, theFeature, theWorkshop, theObjectShapes);
            }
          }
          return;
        }
      }
#ifdef DEBUG_HIDE_COPY_ATTRIBUTE
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
        if (aFeature.get()) {
          AttributeBooleanPtr aCopyAttr = aFeature->data()->boolean(SketchPlugin_SketchEntity::COPY_ID());
          if (aCopyAttr.get()) {
            bool isCopy = aCopyAttr->value();
            if (isCopy)
              return;
          }
        }
      }
#endif
    }

    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult.get())
        aShape = aResult->shape();
    }
    if (!isSubObject(theObject, theFeature))
      appendShapeIfVisible(theWorkshop, theObject, aShape, theObjectShapes);
  }
}

void PartSet_OperationPrs::appendShapeIfVisible(ModuleBase_IWorkshop* theWorkshop,
                              const ObjectPtr& theObject,
                              GeomShapePtr theGeomShape,
                              QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(theWorkshop)->displayer();
  if (XGUI_Displayer::isVisible(aDisplayer, theObject)) {
    if (theGeomShape.get()) {
      if (theObjectShapes.contains(theObject))
        theObjectShapes[theObject].append(theGeomShape);
      else {
        QList<GeomShapePtr> aShapes;
        aShapes.append(theGeomShape);
        theObjectShapes[theObject] = aShapes;
      }
    } else {
  #ifdef DEBUG_EMPTY_SHAPE
      qDebug(QString("Empty shape in result, result: %1")
              .arg(ModuleBase_Tools::objectInfo(theObject)).toStdString().c_str());
  #endif
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
        addValue(aResult, aShape, theFeature, theWorkshop, theObjectShapes);
      }
    }
    if (anAttrType == ModelAPI_AttributeRefList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttribute);
      for (int i = 0; i < aCurSelList->size(); i++) {
        ObjectPtr anObject = aCurSelList->object(i);
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
        // if a feature is stored in the attribute, we should obtain the feature results
        // e.g. feature rectangle uses parameters feature lines in the attribute
        if (aFeature.get()) {
          getResultShapes(aFeature, theWorkshop, theObjectShapes, false);
        }
        else {
          ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
          if (aResult.get()) {
            GeomShapePtr aShape = aResult->shape();
            if (aShape.get())
              addValue(aResult, aShape, theFeature, theWorkshop, theObjectShapes);
          }
        }
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
          AttributePtr anAttribute = anAttr->attr();
          aShape = PartSet_Tools::findShapeBy2DPoint(anAttribute, theWorkshop);
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
      addValue(anObject, aShape, theFeature, theWorkshop, theObjectShapes);
    }
  }
}

void PartSet_OperationPrs::getResultShapes(const FeaturePtr& theFeature,
                                           ModuleBase_IWorkshop* theWorkshop,
                                           QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes,
                                           const bool theListShouldBeCleared)
{
  if (theListShouldBeCleared)
    theObjectShapes.clear();

  if (!theFeature.get())
    return;

  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(theWorkshop)->displayer();

  std::list<ResultPtr> aFeatureResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aRIt = aFeatureResults.begin(),
                                       aRLast = aFeatureResults.end();
  for (; aRIt != aRLast; aRIt++) {
    ResultPtr aResult = *aRIt;
    GeomShapePtr aGeomShape = aResult->shape();
    appendShapeIfVisible(theWorkshop, aResult, aGeomShape, theObjectShapes);
  }
}

void PartSet_OperationPrs::getHighlightedShapes(ModuleBase_IWorkshop* theWorkshop,
                                                QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes)
{
  theObjectShapes.clear();

  QList<ModuleBase_ViewerPrsPtr> aValues;
  ModuleBase_IPropertyPanel* aPanel = theWorkshop->propertyPanel();
  if (aPanel) {
    ModuleBase_ModelWidget* aWidget = aPanel->activeWidget();
    if (aWidget) {
      aWidget->getHighlighted(aValues);
    }
  }

  QList<GeomShapePtr> aShapes;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIIt = aValues.begin(),
                                              aILast = aValues.end();
  for (; anIIt != aILast; anIIt++) {
    ModuleBase_ViewerPrsPtr aPrs = *anIIt;
    ObjectPtr anObject = aPrs->object();

    GeomShapePtr aGeomShape = aPrs->shape();
    if (!aGeomShape.get() || aGeomShape->isNull()) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get()) {
        aGeomShape = aResult->shape();
      }
    }
    appendShapeIfVisible(theWorkshop, anObject, aGeomShape, theObjectShapes);
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

void PartSet_OperationPrs::fillShapeList(const QMap<ObjectPtr, QList<GeomShapePtr> >& theFeatureShapes,
                            ModuleBase_IWorkshop* theWorkshop,
                            NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)>& theShapeToPrsMap)
{
  theShapeToPrsMap.Clear();

  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(theWorkshop)->displayer();

  // create presentations on the base of the shapes
  QMap<ObjectPtr, QList<GeomShapePtr> >::const_iterator anIt = theFeatureShapes.begin(),
                                                        aLast = theFeatureShapes.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = anIt.key();
    QList<GeomShapePtr> aShapes = anIt.value();
    QList<GeomShapePtr>::const_iterator aShIt = aShapes.begin(), aShLast = aShapes.end();
    for (; aShIt != aShLast; aShIt++) {
      GeomShapePtr aGeomShape = *aShIt;
      // the shape should not be checked here on empty value because it should be checked in
      // appendShapeIfVisible() on the step of filling theFeatureShapes list
      // the reason is to avoid empty AIS object visualized in the viewer
      //if (!aGeomShape.get()) continue;
      TopoDS_Shape aShape = aGeomShape.get() ? aGeomShape->impl<TopoDS_Shape>() : TopoDS_Shape();
      if (aShape.IsNull())
        continue;

      // change deviation coefficient to provide more precise circle
      Handle(AIS_InteractiveObject) anIO;
      AISObjectPtr anAISPtr = aDisplayer->getAISObject(anObject);
      if (anAISPtr.get())
        anIO = anAISPtr->impl<Handle(AIS_InteractiveObject)>();
      theShapeToPrsMap.Bind(aShape, anIO);
    }
  }
}

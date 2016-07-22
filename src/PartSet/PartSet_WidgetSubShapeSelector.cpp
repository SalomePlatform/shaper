// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSubShapeSelector.cpp
// Created:     21 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetSubShapeSelector.h"
#include "PartSet_Tools.h"

#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomDataAPI_Point.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <ModelGeomAlgo_Point2D.h>

#include <ModelGeomAlgo_Point2D.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Point.h>

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>

#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QMouseEvent>

PartSet_WidgetSubShapeSelector::PartSet_WidgetSubShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: PartSet_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  //myUseSketchPlane = theData->getBooleanAttribute("use_sketch_plane", true);
  //myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), true);
}

PartSet_WidgetSubShapeSelector::~PartSet_WidgetSubShapeSelector()
{
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                                QMouseEvent* theEvent)
{
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aHighlighted = aSelect->getHighlighted();

  if (aHighlighted.empty()) {
    ModuleBase_ViewerPrsPtr aPrs = aHighlighted.first();
    if (aPrs.get() && aPrs->object().get()) {
      ObjectPtr anObject = aPrs->object();
      if (myCashedShapes.find(anObject) == myCashedShapes.end())
        fillObjectShapes(anObject);
      const std::set<GeomShapePtr>& aShapes = myCashedShapes[anObject];
      if (!aShapes.empty()) {
        gp_Pnt aPnt = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
        std::shared_ptr<GeomAPI_Vertex> aVertexShape(new GeomAPI_Vertex(aPnt.X(), aPnt.Y(), aPnt.Z()));

        std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
        for (; anIt != aLast; anIt++) {
          GeomShapePtr aBaseShape = *anIt;
          if (GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aVertexShape, aBaseShape)) {
            myCurrentSubShape->setObject(anObject);
            myCurrentSubShape->setShape(aBaseShape);
            break;
          }
        }
      }
    }
  }
  myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeHighlightedObjects,
                                        true);
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::getHighlighted(
                           QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues)
{
  if (myCurrentSubShape.get() && myCurrentSubShape->object().get())
    theValues.append(myCurrentSubShape);
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::fillObjectShapes(const ObjectPtr& theObject)
{
  std::set<std::shared_ptr<GeomAPI_Shape> > aShapes;

  // current feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  std::set<GeomShapePtr> anEdgeShapes;
  // edges on feature
  ModelAPI_Tools::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
  if (!anEdgeShapes.empty()) {
    GeomShapePtr aFeatureShape = *anEdgeShapes.begin();

    // coincidences to the feature
    std::set<std::shared_ptr<GeomDataAPI_Point2D> > aRefAttributes;
    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    FeaturePtr aSketch = sketch();
    std::shared_ptr<ModelAPI_Data> aData = aSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));
    std::set<std::shared_ptr<GeomAPI_Pnt> > aPoints;
    ModelGeomAlgo_Point2D::getPointsInsideShape(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPoints);

    GeomAlgoAPI_ShapeTools::splitShape(aFeatureShape, aPoints, aShapes);
  }
  myCashedShapes[theObject] = aShapes;
}

//********************************************************************
/*bool PartSet_WidgetSubShapeSelector::activateSelectionAndFilters(bool toActivate)
{
  bool aHasSelectionFilter = ModuleBase_WidgetShapeSelector::activateSelectionAndFilters
                                                                           (toActivate);
  if (!myUseSketchPlane) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(aWorkshop->module());
    bool isUsePlaneFilterOnly = !toActivate;
    aModule->sketchMgr()->activatePlaneFilter(isUsePlaneFilterOnly);
  }
  return aHasSelectionFilter;
}

//********************************************************************
bool PartSet_WidgetSubShapeSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = ModuleBase_WidgetShapeSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

void PartSet_WidgetSubShapeSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetShapeSelector::getGeomSelection(thePrs, theObject, theShape);

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists, try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetSubShapeSelector
  if (aSPFeature.get() == NULL) {
    ObjectPtr anExternalObject = ObjectPtr();
    if (myExternalObjectMgr->useExternal()) {
      GeomShapePtr aShape = theShape;
      if (!aShape.get()) {
        ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
        if (aResult.get())
          aShape = aResult->shape();
      }
      if (aShape.get() != NULL && !aShape->isNull())
        anExternalObject = myExternalObjectMgr->externalObject(theObject, aShape, sketch(), myIsInValidate);
    }
    /// the object is null if the selected feature is "external"(not sketch entity feature of the
    /// current sketch) and it is not created by object manager
    theObject = anExternalObject;
  }
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::restoreAttributeValue(const AttributePtr& theAttribute,
                                                        const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theAttribute, theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);
}
*/

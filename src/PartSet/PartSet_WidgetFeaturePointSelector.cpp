// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetFeaturePointSelector.cpp
// Created:     28 Feb 2017
// Author:      Natalia ERMOLAEVA

#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>

#include "PartSet_WidgetFeaturePointSelector.h"
#include "PartSet_Tools.h"

//#include <SketchPlugin_ConstraintCoincidence.h>
//#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Trim.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_ViewerProxy.h>

#include <QWidget>
#include <QMouseEvent>

#define HIGHLIGHT_STAYS_PROBLEM
#ifdef HIGHLIGHT_STAYS_PROBLEM
#include <Quantity_Color.hxx>
#define SKETCH_ENTITY_COLOR "225,0,0"
#endif

PartSet_WidgetFeaturePointSelector::PartSet_WidgetFeaturePointSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
}

PartSet_WidgetFeaturePointSelector::~PartSet_WidgetFeaturePointSelector()
{
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::isValidSelection(
                                        const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  return ModuleBase_WidgetShapeSelector::isValidSelection(theValue);
  //return true;
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::activateSelectionAndFilters(bool toActivate)
{
#ifdef HIGHLIGHT_STAYS_PROBLEM
  Handle(AIS_InteractiveContext) aContext =
                            XGUI_Tools::workshop(myWorkshop)->viewer()->AISContext();
  Quantity_Color aColor;
  Handle(Graphic3d_HighlightStyle) aHStyle = aContext->HighlightStyle();
  Handle(Graphic3d_HighlightStyle) aSStyle = aContext->SelectionStyle();
  if (toActivate) {
    std::vector<int> aColors;
    aColors = Config_PropManager::color("Visualization", "sketch_entity_color");
    aColor = Quantity_Color(aColors[0] / 255., aColors[1] / 255., aColors[2] / 255.,
                            Quantity_TOC_RGB);
    myHighlightColor = aHStyle->Color();
    mySelectionColor = aSStyle->Color();
  }
  else {
    aColor = myHighlightColor;
  }
  aHStyle->SetColor(aColor);
  aContext->SetHighlightStyle(aHStyle);

  aSStyle->SetColor(aColor);
  aContext->SetSelectionStyle(aSStyle);

#endif

  return ModuleBase_WidgetShapeSelector::activateSelectionAndFilters(toActivate);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::activateCustom()
{
  ModuleBase_WidgetShapeSelector::activateCustom();

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::deactivate()
{
  ModuleBase_WidgetShapeSelector::deactivate();
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                                    QMouseEvent* theEvent)
{
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aHighlighted = aSelect->getHighlighted();

  ModuleBase_ViewerPrsPtr aPrs = !aHighlighted.empty() ? aHighlighted.first()
                                                       : ModuleBase_ViewerPrsPtr();
  fillFeature(aPrs, theWindow, theEvent);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseReleased(ModuleBase_IViewWindow* theWindow,
                                                       QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  // set parameters of preview into parameters of selection in the feature
  std::shared_ptr<ModelAPI_AttributeReference> aRefSelectedAttr =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  std::shared_ptr<ModelAPI_AttributeReference> aRefPreviewAttr =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(SketchPlugin_Trim::PREVIEW_OBJECT()));
  aRefSelectedAttr->setValue(aRefPreviewAttr->value());

  std::shared_ptr<GeomDataAPI_Point2D> aPointSelectedAttr =
                          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                          feature()->data()->attribute(SketchPlugin_Trim::SELECTED_POINT()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointPreviewAttr =
                          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                          feature()->data()->attribute(SketchPlugin_Trim::PREVIEW_POINT()));
  aPointSelectedAttr->setValue(aPointPreviewAttr->x(), aPointPreviewAttr->y());

  updateObject(feature());

  emit focusOutWidget(this);
  // we need to deselect base feature for better visibility of selected feature
  XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(false);
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::fillFeature(
                            const std::shared_ptr<ModuleBase_ViewerPrs>& theSelectedPrs,
                            ModuleBase_IViewWindow* theWindow,
                            QMouseEvent* theEvent)
{
  bool aFilled = false;
  ObjectPtr anObject;
  if (theSelectedPrs.get() && theSelectedPrs->object().get())
    anObject = theSelectedPrs->object();

  if (!anObject.get())
    return aFilled;

  gp_Pnt aPnt = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
  double aX, anY;
  Handle(V3d_View) aView = theWindow->v3dView();
  PartSet_Tools::convertTo2D(aPnt, mySketch, aView, aX, anY);

  std::shared_ptr<ModelAPI_AttributeReference> aRef =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(SketchPlugin_Trim::PREVIEW_OBJECT()));
  aRef->setValue(anObject);

  std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
                  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                  feature()->data()->attribute(SketchPlugin_Trim::PREVIEW_POINT()));
  anAttributePoint->setValue(aX, anY);
  // redisplay AIS presentation in viewer
#ifndef HIGHLIGHT_STAYS_PROBLEM
  // an attempt to clear highlighted item in the viewer: but of OCCT
  XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(true);
#endif
  updateObject(feature());
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

  aFilled = true;

  return aFilled;
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> PartSet_WidgetFeaturePointSelector::getAttributeSelection() const
{
  return QList<ModuleBase_ViewerPrsPtr>();
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::setSelection(
                                          QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                                          const bool theToValidate)
{
  // false is returned to do not emit focus out widget by selected sub-shape
  return false;
}

void PartSet_WidgetFeaturePointSelector::setPreSelection(
                                  const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                                  ModuleBase_IViewWindow* theWnd,
                                  QMouseEvent* theEvent)
{
  if (fillFeature(thePreSelected, theWnd, theEvent))
    mouseReleased(theWnd, theEvent);
}

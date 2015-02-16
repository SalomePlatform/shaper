// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_Tools.h"

#include <ModuleBase_DoubleSpinBox.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_Tools.h>

#include <XGUI_ViewerProxy.h>
#include <XGUI_Workshop.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_OperationMgr.h>

#include <GeomAPI_Pnt2d.h>
#include <Config_WidgetAPI.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <QMouseEvent>

PartSet_WidgetPoint2dDistance::PartSet_WidgetPoint2dDistance(QWidget* theParent,
                                                                   const Config_WidgetAPI* theData,
                                                                   const std::string& theParentId)
    : ModuleBase_WidgetDoubleValue(theParent, theData, theParentId)
{
  myFirstPntName = theData->getProperty("first_point");

  // Reconnect to local slot
  disconnect(mySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
  connect(mySpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValuesChanged()));
}

PartSet_WidgetPoint2dDistance::~PartSet_WidgetPoint2dDistance()
{
}

void PartSet_WidgetPoint2dDistance::reset()
{
  bool isOk;
  double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);

  ModuleBase_Tools::setSpinValue(mySpinBox, isOk ? aDefValue : 0.0);
}

void PartSet_WidgetPoint2dDistance::setPoint(FeaturePtr theFeature,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePnt)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(myFirstPntName));
  if (!aPoint)
    return;

  double aRadius = thePnt->distance(aPoint->pnt());
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal && (aReal->value() != aRadius)) {
    aReal->setValue(aRadius);
    
    ModuleBase_Tools::setSpinValue(mySpinBox, aRadius);
    storeValue();
  }
}

void PartSet_WidgetPoint2dDistance::activateCustom()
{
  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));
}

void PartSet_WidgetPoint2dDistance::deactivate()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  disconnect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  disconnect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));
  myWorkshop->operationMgr()->setLockValidating(false);
}

void PartSet_WidgetPoint2dDistance::onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, aY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, aY);

  std::shared_ptr<GeomAPI_Pnt2d> aPnt = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
  setPoint(feature(), aPnt);
  emit focusOutWidget(this);
}

void PartSet_WidgetPoint2dDistance::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  myWorkshop->operationMgr()->setLockValidating(true);
  myWorkshop->operationMgr()->setApplyEnabled(false);

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, aY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, aY);

  std::shared_ptr<GeomAPI_Pnt2d> aPnt = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
  setPoint(feature(), aPnt);
}

void PartSet_WidgetPoint2dDistance::onValuesChanged()
{
  myWorkshop->operationMgr()->setLockValidating(false);
  emit valuesChanged();
}


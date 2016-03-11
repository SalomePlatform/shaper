// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dDistance.cpp
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_Tools.h"

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>

#include <GeomAPI_Pnt2d.h>
#include <Config_WidgetAPI.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <QMouseEvent>

PartSet_WidgetPoint2dDistance::PartSet_WidgetPoint2dDistance(QWidget* theParent,
                                                             ModuleBase_IWorkshop* theWorkshop,
                                                             const Config_WidgetAPI* theData)
: ModuleBase_WidgetDoubleValue(theParent, theData), myWorkshop(theWorkshop),
  myValueIsCashed(false), myIsFeatureVisibleInCash(true), myValueInCash(0)
{
  myFirstPntName = theData->getProperty("first_point");
}

PartSet_WidgetPoint2dDistance::~PartSet_WidgetPoint2dDistance()
{
}

bool PartSet_WidgetPoint2dDistance::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  }
  else {
    if (myValueIsCashed) {
      // if the restored value should be hidden, aDone = true to set
      // reset state for the widget in the parent
      aDone = restoreCurentValue();
    }
    else
      aDone = ModuleBase_WidgetDoubleValue::resetCustom();
  }
  return aDone;
}

void PartSet_WidgetPoint2dDistance::setPoint(FeaturePtr theFeature,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePnt)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(myFirstPntName));
  if (!aPoint)
    return;

  double aValue = computeValue(aPoint->pnt(), thePnt);
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal && (aReal->value() != aValue)) {
    aReal->setValue(aValue);
    
    ModuleBase_Tools::setSpinValue(mySpinBox, aValue);
    storeValue();
  }
}

double PartSet_WidgetPoint2dDistance::computeValue(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPnt,
                                                   const std::shared_ptr<GeomAPI_Pnt2d>& theCurrentPnt)
{
  return theCurrentPnt->distance(theFirstPnt);
}

void PartSet_WidgetPoint2dDistance::activateCustom()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));
}

void PartSet_WidgetPoint2dDistance::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  disconnect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  disconnect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));
}

void PartSet_WidgetPoint2dDistance::onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  if (mySpinBox->hasVariable())
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, aY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, aY);

  std::shared_ptr<GeomAPI_Pnt2d> aPnt = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
  setPoint(feature(), aPnt);

  // if the validator of the control returns false, focus should not be switched
  if (getError().isEmpty())
    emit focusOutWidget(this);
}

void PartSet_WidgetPoint2dDistance::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (isEditingMode())
    return;

  if (mySpinBox->hasVariable())
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, aY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, aY);

  std::shared_ptr<GeomAPI_Pnt2d> aPnt = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
  if (myState != ModifiedInViewer)
    storeCurentValue();

  bool isBlocked = blockValueState(true);
  setPoint(feature(), aPnt);
  blockValueState(isBlocked);
  setValueState(ModifiedInViewer);
}

void PartSet_WidgetPoint2dDistance::storeCurentValue()
{
  // do not use cash if a variable is used
  if (mySpinBox->hasVariable())
    return;

  myValueIsCashed = true;
  myIsFeatureVisibleInCash = XGUI_Displayer::isVisible(
                       XGUI_Tools::workshop(myWorkshop)->displayer(), myFeature);
  myValueInCash = mySpinBox->value();
}

bool PartSet_WidgetPoint2dDistance::restoreCurentValue()
{
  bool aRestoredAndHidden = true;

  bool isVisible = myIsFeatureVisibleInCash;
  // fill the control widgets by the cashed value

  myValueIsCashed = false;
  myIsFeatureVisibleInCash = true;
  ModuleBase_Tools::setSpinValue(mySpinBox, myValueInCash);

  // store value to the model
  storeValueCustom();
  if (isVisible) {
    setValueState(Stored);
    aRestoredAndHidden = false;
  }
  else
    aRestoredAndHidden = true;

  return aRestoredAndHidden;
}

bool PartSet_WidgetPoint2dDistance::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    mySpinBox->selectAll();
  }
  return isModified;
}

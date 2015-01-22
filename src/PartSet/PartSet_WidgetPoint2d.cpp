// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2d.h"
#include <PartSet_Tools.h>

#include <XGUI_Workshop.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_OperationMgr.h>

#include <ModuleBase_DoubleSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewWindow.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>

#include <cfloat>
#include <climits>

PartSet_WidgetPoint2D::PartSet_WidgetPoint2D(QWidget* theParent, 
                                              const Config_WidgetAPI* theData,
                                              const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  //myOptionParam = theData->getProperty(PREVIOUS_FEATURE_PARAM);
  QString aPageName = QString::fromStdString(theData->getProperty(CONTAINER_PAGE_NAME));
  myGroupBox = new QGroupBox(aPageName, theParent);
  myGroupBox->setFlat(false);

  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(aGroupLay);
  aGroupLay->setColumnStretch(1, 1);
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("X"));
    aLabel->setPixmap(QPixmap(":pictures/x_point.png"));
    aGroupLay->addWidget(aLabel, 0, 0);

    myXSpin = new ModuleBase_DoubleSpinBox(myGroupBox);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip(tr("X"));
    aGroupLay->addWidget(myXSpin, 0, 1);

    connect(myXSpin, SIGNAL(valueChanged(double)), this, SLOT(onValuesChanged()));
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("Y"));
    aLabel->setPixmap(QPixmap(":pictures/y_point.png"));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new ModuleBase_DoubleSpinBox(myGroupBox);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip(tr("Y"));
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(double)), this, SLOT(onValuesChanged()));
  }
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
}

bool PartSet_WidgetPoint2D::setSelection(ModuleBase_ViewerPrs theValue)
{
  Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
  bool isDone = false;
  TopoDS_Shape aShape = theValue.shape();
  double aX, aY;
  if (getPoint2d(aView, aShape, aX, aY)) {
    setPoint(aX, aY);
    isDone = true;
  }
  return isDone;
}

void PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{

  bool isBlocked = this->blockSignals(true);
  myXSpin->blockSignals(true);
  myXSpin->setValue(theX);
  myXSpin->blockSignals(false);

  myYSpin->blockSignals(true);
  myYSpin->setValue(theY);
  myYSpin->blockSignals(false);
  this->blockSignals(isBlocked);

  emit valuesChanged();
}

bool PartSet_WidgetPoint2D::storeValue() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData) // can be on abort of sketcher element
    return false;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  
  PartSet_WidgetPoint2D* that = (PartSet_WidgetPoint2D*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPoint->setImmutable(true);
#ifdef _DEBUG
  std::string _attr_name = myAttributeID;
  double _X = myXSpin->value();
  double _Y = myYSpin->value();
#endif
  aPoint->setValue(myXSpin->value(), myYSpin->value());
  // after movement the solver will call the update event: optimization
  moveObject(myFeature);
  aPoint->setImmutable(isImmutable);
  that->blockSignals(isBlocked);

  return true;
}

bool PartSet_WidgetPoint2D::restoreValue()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));

#ifdef _DEBUG
  std::string _attr_name = myAttributeID;
  double _X = aPoint->x();
  double _Y = aPoint->y();
#endif
  bool isBlocked = this->blockSignals(true);
  myXSpin->blockSignals(true);
  myXSpin->setValue(aPoint->x());
  myXSpin->blockSignals(false);

  myYSpin->blockSignals(true);
  myYSpin->setValue(aPoint->y());
  myYSpin->blockSignals(false);
  this->blockSignals(isBlocked);
  return true;
}

QWidget* PartSet_WidgetPoint2D::getControl() const
{
  return myGroupBox;
}

QList<QWidget*> PartSet_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myXSpin);
  aControls.append(myYSpin);
  return aControls;
}


void PartSet_WidgetPoint2D::activate()
{
  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  QIntList aModes;
  aModes << TopAbs_VERTEX;
  myWorkshop->moduleConnector()->activateSubShapesSelection(aModes);
}

void PartSet_WidgetPoint2D::deactivate()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  disconnect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  disconnect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));
  myWorkshop->moduleConnector()->deactivateSubShapesSelection();
  myWorkshop->operationMgr()->setLockValidating(false);
}

bool PartSet_WidgetPoint2D::getPoint2d(const Handle(V3d_View)& theView, 
                                       const TopoDS_Shape& theShape, 
                                       double& theX, double& theY) const
{
  if (!theShape.IsNull()) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
      if (!aVertex.IsNull()) {
        // A case when point is taken from existing vertex
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        PartSet_Tools::convertTo2D(aPoint, mySketch, theView, theX, theY);
        return true;
      }
    }
  }
  return false;
}


void PartSet_WidgetPoint2D::onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  // TODO: This fragment doesn't work because bug in OCC Viewer. It can be used after fixing.
  //NCollection_List<TopoDS_Shape> aShapes;
  //std::list<ObjectPtr> aObjects;
  //aSelection->selectedShapes(aShapes, aObjects);
  //if (aShapes.Extent() > 0) {
  //  TopoDS_Shape aShape = aShapes.First();
  //  double aX, aY;
  //  if (getPoint2d(theWnd->v3dView(), aShape, aX, aY)) {
  //    setPoint(aX, aY);

  //    PartSet_Tools::setConstraints(mySketch, feature(), attributeID(),aX, aY);
  //    emit vertexSelected(aObjects.front(), aShape);
  //    emit focusOutWidget(this);
  //    return;
  //  }
  //}
  // End of Bug dependent fragment

  // A case when point is taken from mouse event
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());
  double aX, anY;
  Handle(V3d_View) aView = theWnd->v3dView();
  PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, anY);
  //setPoint(aX, anY);

  std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(feature()->data()->attribute(attributeID()));
  QList<FeaturePtr> aIgnore;
  aIgnore.append(feature());

  double aTolerance = aView->Convert(7);
  std::shared_ptr<GeomDataAPI_Point2D> aAttrPnt = 
    PartSet_Tools::findAttributePoint(mySketch, aX, anY, aTolerance, aIgnore);
  if (aAttrPnt.get() != NULL) {
    aFeaturePoint->setValue(aAttrPnt->pnt());
    PartSet_Tools::createConstraint(mySketch, aAttrPnt, aFeaturePoint);
    emit vertexSelected();
  }
  emit focusOutWidget(this);
}


void PartSet_WidgetPoint2D::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (isEditingMode())
    return;
  myWorkshop->operationMgr()->setLockValidating(true);
  myWorkshop->propertyPanel()->setOkEnabled(false);

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, anY);
  setPoint(aX, anY);
}

double PartSet_WidgetPoint2D::x() const
{
  return myXSpin->value();
}

double PartSet_WidgetPoint2D::y() const
{
  return myYSpin->value();
}

void PartSet_WidgetPoint2D::onValuesChanged()
{
  myWorkshop->operationMgr()->setLockValidating(false);
  emit valuesChanged();
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2d.h"
#include <PartSet_Tools.h>
#include <PartSet_Module.h>
#include <PartSet_LockApplyMgr.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_ISelection.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_ConstraintCoincidence.h>

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

const double MaxCoordinate = 1e12;


PartSet_WidgetPoint2D::PartSet_WidgetPoint2D(QWidget* theParent, 
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const Config_WidgetAPI* theData,
                                             const std::string& theParentId)
 : ModuleBase_ModelWidget(theParent, theData, theParentId), myWorkshop(theWorkshop)
{
  myLockApplyMgr = new PartSet_LockApplyMgr(theParent, myWorkshop);

  // the control should accept the focus, so the boolen flag is corrected to be true
  myIsObligatory = true;
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

    myXSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip(tr("X"));
    aGroupLay->addWidget(myXSpin, 0, 1);

    connect(myXSpin, SIGNAL(valueChanged(const QString&)), this, SLOT(onValuesChanged()));
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("Y"));
    aLabel->setPixmap(QPixmap(":pictures/y_point.png"));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip(tr("Y"));
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(const QString&)), this, SLOT(onValuesChanged()));
  }
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myGroupBox);
  setLayout(aLayout);
}

void PartSet_WidgetPoint2D::reset()
{
  if (!isUseReset())
    return;

  if (isComputedDefault() || myXSpin->hasVariable() || myYSpin->hasVariable()) {
    return;
    //if (myFeature->compute(myAttributeID))
    //  restoreValue();
  }
  else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
    // it is important to block the spin box control in order to do not through out the
    // locking of the validating state.
    ModuleBase_Tools::setSpinValue(myXSpin, isOk ? aDefValue : 0.0);
    ModuleBase_Tools::setSpinValue(myYSpin, isOk ? aDefValue : 0.0);
    storeValueCustom();
  }
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
}

bool PartSet_WidgetPoint2D::setSelection(QList<ModuleBase_ViewerPrs>& theValues)
{
  if (theValues.empty())
    return false;

  ModuleBase_ViewerPrs aValue = theValues.takeFirst();

  Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
  bool isDone = false;
  TopoDS_Shape aShape = aValue.shape();
  double aX, aY;
  if (getPoint2d(aView, aShape, aX, aY)) {
    isDone = setPoint(aX, aY);
  }
  return isDone;
}

bool PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{
  if (fabs(theX) >= MaxCoordinate)
    return false;
  if (fabs(theY) >= MaxCoordinate)
    return false;

  ModuleBase_Tools::setSpinValue(myXSpin, theX);
  ModuleBase_Tools::setSpinValue(myYSpin, theY);

  storeValue();
  return true;
}

bool PartSet_WidgetPoint2D::storeValueCustom() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData) // can be on abort of sketcher element
    return false;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  
  PartSet_WidgetPoint2D* that = (PartSet_WidgetPoint2D*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPoint->setImmutable(true);
  
  if (myXSpin->hasVariable() || myYSpin->hasVariable()) {
    aPoint->setText(myXSpin->text().toStdString(), myYSpin->text().toStdString());
  } else {
    aPoint->setValue(myXSpin->value(), myYSpin->value());
    aPoint->setText("", "");
  }
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
  std::string aTextX = aPoint->textX();
  std::string aTextY = aPoint->textY();
  if (aTextX.empty() || aTextY.empty()) {
    double aX = aPoint->x();
    ModuleBase_Tools::setSpinValue(myXSpin, aPoint->x());
    ModuleBase_Tools::setSpinValue(myYSpin, aPoint->y());
  } else {
    ModuleBase_Tools::setSpinText(myXSpin, QString::fromStdString(aTextX));
    ModuleBase_Tools::setSpinText(myYSpin, QString::fromStdString(aTextY));
  }
  return true;
}

QList<QWidget*> PartSet_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myXSpin);
  aControls.append(myYSpin);
  return aControls;
}


void PartSet_WidgetPoint2D::activateCustom()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  QIntList aModes;
  aModes << TopAbs_VERTEX;
  aModes << TopAbs_EDGE;
  myWorkshop->activateSubShapesSelection(aModes);

  myLockApplyMgr->activate();
}

void PartSet_WidgetPoint2D::deactivate()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  disconnect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
             this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  disconnect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  myWorkshop->deactivateSubShapesSelection();

  myLockApplyMgr->deactivate();
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
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  Handle(V3d_View) aView = theWnd->v3dView();
  // TODO: This fragment doesn't work because bug in OCC Viewer. It can be used after fixing.
  NCollection_List<TopoDS_Shape> aShapes;
  std::list<ObjectPtr> aObjects;
  aSelection->selectedShapes(aShapes, aObjects);
  // if we have selection
  if (aShapes.Extent() > 0) {
    TopoDS_Shape aShape = aShapes.First();
    ObjectPtr aObject = aObjects.front();
    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aObject);
    if (aSelectedFeature.get() != NULL) {
      std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
      if ((!aSPFeature) && (!aShape.IsNull())) {
        ResultPtr aFixedObject = PartSet_Tools::findFixedObjectByExternal(aShape, aObject, mySketch);
        if (!aFixedObject.get())
          aFixedObject = PartSet_Tools::createFixedObjectByExternal(aShape, aObject, mySketch);
      }
    }
    double aX, aY;
    if (getPoint2d(aView, aShape, aX, aY)) {
      setPoint(aX, aY);

      PartSet_Tools::setConstraints(mySketch, feature(), attributeID(),aX, aY);
      emit vertexSelected();
      emit focusOutWidget(this);
      return;
    } else if (aShape.ShapeType() == TopAbs_EDGE) {
      // Create point-edge coincedence
      FeaturePtr aFeature = mySketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
      AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
      std::shared_ptr<GeomDataAPI_Point2D> aThisPoint = 
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
      aRef1->setAttr(aThisPoint);

      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
      aRef2->setObject(aObject);
      aFeature->execute();
      emit vertexSelected();
      emit focusOutWidget(this);
      return;
    }
  }
  // End of Bug dependent fragment

  // A case when point is taken from mouse event
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());
  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, anY);
  if (!setPoint(aX, anY))
    return;

  /// Start alternative code
  //std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = std::dynamic_pointer_cast<
  //    GeomDataAPI_Point2D>(feature()->data()->attribute(attributeID()));
  //QList<FeaturePtr> aIgnore;
  //aIgnore.append(feature());

  //double aTolerance = aView->Convert(7);
  //std::shared_ptr<GeomDataAPI_Point2D> aAttrPnt = 
  //  PartSet_Tools::findAttributePoint(mySketch, aX, anY, aTolerance, aIgnore);
  //if (aAttrPnt.get() != NULL) {
  //  aFeaturePoint->setValue(aAttrPnt->pnt());
  //  PartSet_Tools::createConstraint(mySketch, aAttrPnt, aFeaturePoint);
  //  emit vertexSelected();
  //}
  /// End alternative code
  emit focusOutWidget(this);
}


void PartSet_WidgetPoint2D::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (isEditingMode())
    return;

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
  myLockApplyMgr->valuesChanged();
  emit valuesChanged();
}

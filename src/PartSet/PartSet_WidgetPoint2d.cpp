// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2D.h"
#include "PartSet_Tools.h"

#include <XGUI_Workshop.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>

#include <ModuleBase_WidgetValueFeature.h>
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
    aLabel->setText("X");
    aLabel->setPixmap(QPixmap(":pictures/x_point.png"));
    aGroupLay->addWidget(aLabel, 0, 0);

    myXSpin = new ModuleBase_DoubleSpinBox(myGroupBox);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip("X");
    aGroupLay->addWidget(myXSpin, 0, 1);

    connect(myXSpin, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText("Y");
    aLabel->setPixmap(QPixmap(":pictures/y_point.png"));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new ModuleBase_DoubleSpinBox(myGroupBox);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip("X");
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
  }
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
}

bool PartSet_WidgetPoint2D::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;
  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue =
        dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aFeatureValue->point();
      if (aPoint) {
        setPoint(aPoint->x(), aPoint->y());
        isDone = true;
      }
    }
  }
  return isDone;
}

void PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{

  bool isBlocked = this->blockSignals(true);
  myXSpin->setValue(theX);
  myYSpin->setValue(theY);
  this->blockSignals(isBlocked);

  emit valuesChanged();
}

bool PartSet_WidgetPoint2D::storeValue() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
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
  updateObject(myFeature);
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
  myXSpin->setValue(aPoint->x());
  myYSpin->setValue(aPoint->y());
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

//bool PartSet_WidgetPoint2D::initFromPrevious(ObjectPtr theObject)
//{
//  if (myOptionParam.length() == 0)
//    return false;
//  std::shared_ptr<ModelAPI_Data> aData = theObject->data();
//  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
//      aData->attribute(myOptionParam));
//  if (aPoint) {
//    bool isBlocked = this->blockSignals(true);
//    myXSpin->setValue(aPoint->x());
//    myYSpin->setValue(aPoint->y());
//    this->blockSignals(isBlocked);
//
//    emit valuesChanged();
//    emit storedPoint2D(theObject, myOptionParam);
//    return true;
//  }
//  return false;
//}

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
}


void PartSet_WidgetPoint2D::onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  NCollection_List<TopoDS_Shape> aShapes;
  std::list<ObjectPtr> aObjects;
  aSelection->selectedShapes(aShapes, aObjects);
  if (aShapes.Extent() > 0) {
    TopoDS_Shape aShape = aShapes.First();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          // A case when point is taken from existing vertex
          gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
          double aX, aY;
          PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, aY);
          setPoint(aX, aY);

          PartSet_Tools::setConstraints(mySketch, feature(), attributeID(),aX, aY);
          emit vertexSelected(aObjects.front(), aShape);
          QApplication::processEvents();
          emit focusOutWidget(this);
          return;
        }
      }
    }
  }
  // A case when point is taken from mouse event
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());
  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, anY);
  setPoint(aX, anY);

  emit focusOutWidget(this);
}


void PartSet_WidgetPoint2D::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
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


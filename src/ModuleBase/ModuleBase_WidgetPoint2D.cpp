// File:        ModuleBase_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_DoubleSpinBox.h>
#include <ModuleBase_Tools.h>

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

#include <cfloat>
#include <climits>

ModuleBase_WidgetPoint2D::ModuleBase_WidgetPoint2D(QWidget* theParent,
                                                   const Config_WidgetAPI* theData,
                                                   const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myOptionParam = theData->getProperty(PREVIOUS_FEATURE_PARAM);
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

ModuleBase_WidgetPoint2D::~ModuleBase_WidgetPoint2D()
{
}

bool ModuleBase_WidgetPoint2D::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;
  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue =
        dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aFeatureValue->point();
      if (aPoint) {
        setPoint(aPoint);
        isDone = true;
      }
    }
  }
  return isDone;
}

void ModuleBase_WidgetPoint2D::setPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{

  bool isBlocked = this->blockSignals(true);
  myXSpin->setValue(thePoint->x());
  myYSpin->setValue(thePoint->y());
  this->blockSignals(isBlocked);

  emit valuesChanged();
}

bool ModuleBase_WidgetPoint2D::storeValue() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  
  ModuleBase_WidgetPoint2D* that = (ModuleBase_WidgetPoint2D*) this;
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

bool ModuleBase_WidgetPoint2D::restoreValue()
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

QWidget* ModuleBase_WidgetPoint2D::getControl() const
{
  return myGroupBox;
}

QList<QWidget*> ModuleBase_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.push_back(myXSpin);
  aControls.push_back(myYSpin);

  return aControls;
}

bool ModuleBase_WidgetPoint2D::initFromPrevious(ObjectPtr theObject)
{
  if (myOptionParam.length() == 0)
    return false;
  std::shared_ptr<ModelAPI_Data> aData = theObject->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(myOptionParam));
  if (aPoint) {
    bool isBlocked = this->blockSignals(true);
    myXSpin->setValue(aPoint->x());
    myYSpin->setValue(aPoint->y());
    this->blockSignals(isBlocked);

    emit valuesChanged();
    emit storedPoint2D(theObject, myOptionParam);
    return true;
  }
  return false;
}

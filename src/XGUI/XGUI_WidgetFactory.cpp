/*
 * XGUI_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <XGUI_WidgetFactory.h>
#include <ModuleBase_Operation.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QMetaProperty>
#include <QLabel>
#include <QPixmap>

#ifdef _DEBUG
#include <QDebug>
#endif

XGUI_WidgetFactory::XGUI_WidgetFactory(ModuleBase_Operation* theOperation)
    : myOperation(theOperation)
{
  QString aXml = myOperation->xmlRepresentation();
  myWidgetApi = new Config_WidgetAPI(aXml.toStdString());
}

XGUI_WidgetFactory::~XGUI_WidgetFactory()
{
}

void XGUI_WidgetFactory::fillWidget(QWidget* theParent)
{
  myWidgetApi->reset();
  if (theParent->layout()) {
    theParent->layout()->deleteLater();
  }

  QVBoxLayout* aWidgetLay = new QVBoxLayout(theParent);
  aWidgetLay->setContentsMargins(0, 0, 0, 0);
  do {
    std::string aWdgType = myWidgetApi->widgetType();
    QWidget* aWidget = NULL;
    if (aWdgType == NODE_DOUBLE_WDG) {
      aWidget = doubleSpinBoxWidget();
    } else {
      #ifdef _DEBUG
      qDebug() << "XGUI_WidgetFactory::fillWidget: find bad widget type";
      #endif
    }
    if (aWidget) {
      aWidgetLay->addWidget(aWidget);
    }
  } while(myWidgetApi->nextWidget());
  theParent->setLayout(aWidgetLay);
}

QWidget* XGUI_WidgetFactory::doubleSpinBoxWidget()
{
  QWidget* result = new QWidget();
  QHBoxLayout* aControlLay = new QHBoxLayout(result);
  aControlLay->setContentsMargins(0, 0, 0, 0);
  QString aLabelText = qs(myWidgetApi->widgetLabel());
  QString aLabelIcon = qs(myWidgetApi->widgetIcon());
  QLabel* aLabel = new QLabel(aLabelText);
  aLabel->setPixmap(QPixmap(aLabelIcon));

  aControlLay->addWidget(aLabel);
  QDoubleSpinBox* aBox = new QDoubleSpinBox(result);
  QString anObjName = QString::fromStdString(myWidgetApi->widgetId());
  aBox->setObjectName(anObjName);
  bool isOk = false;
  std::string aProp = myWidgetApi->getProperty(DOUBLE_WDG_MIN);
  double aMinVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setMinimum(aMinVal);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setMaximum(aMaxVal);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_STEP);
  double aStepVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setSingleStep(aStepVal);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_DFLT);
  double aDefVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setValue(aDefVal);
  }
  QString aTTip = qs(myWidgetApi->widgetTooltip());
  aBox->setToolTip(aTTip);
  aControlLay->addWidget(aBox);
  aControlLay->setStretch(1, 1);
  result->setLayout(aControlLay);
  connectWidget(aBox, NODE_DOUBLE_WDG);
  return result;
}

bool XGUI_WidgetFactory::connectWidget(QWidget* theWidget, const QString& theType)
{
  bool result = false;
  if (theType == NODE_DOUBLE_WDG) {
    result = QObject::connect(theWidget, SIGNAL(valueChanged(double)), 
                              myOperation, SLOT(storeReal(double)));
  }
  return result;
}

QString XGUI_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}

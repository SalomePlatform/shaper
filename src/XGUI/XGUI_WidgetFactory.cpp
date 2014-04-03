/*
 * XGUI_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <XGUI_WidgetFactory.h>

#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QMetaProperty>
#include <QLabel>

#ifdef _DEBUG
#include <QDebug>
#endif

XGUI_WidgetFactory::XGUI_WidgetFactory(const std::string& theXml)
{
  myWidgetApi = new Config_WidgetAPI(theXml);
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
  do {
    std::string aWdgType = myWidgetApi->widgetType();
    QWidget* aWidget = NULL;
    if (aWdgType == "value") {
      aWidget = valueWidget();
    } else {
      #ifdef _DEBUG
      qDebug() << "XGUI_WidgetFactory::fillWidget: find bad widget type";
      #endif
    }
    if (aWidget) {
      aWidgetLay->addWidget(aWidget);
    }
  } while(myWidgetApi->nextWidget());
  aWidgetLay->addStretch(1);
  theParent->setLayout(aWidgetLay);
}

QWidget* XGUI_WidgetFactory::valueWidget()
{
  QWidget* result = new QWidget();
  QHBoxLayout* aControlLay = new QHBoxLayout(result);
  QString aLabelVal = qs(myWidgetApi->getProperty("label"));
  QLabel* aLabel = new QLabel(aLabelVal);
  aControlLay->addWidget(aLabel);
  QDoubleSpinBox* aBox = new QDoubleSpinBox(result);
  bool isOk = false;
  double aMinVal = qs(myWidgetApi->getProperty("min")).toDouble(&isOk);
  if (isOk) {
    aBox->setMinimum(aMinVal);
  }
  double aMaxVal = qs(myWidgetApi->getProperty("max")).toDouble(&isOk);
  if (isOk) {
    aBox->setMaximum(aMaxVal);
  }
  double aStepVal = qs(myWidgetApi->getProperty("step")).toDouble(&isOk);
  if (isOk) {
    aBox->setSingleStep(aStepVal);
  }
  double aDefVal = qs(myWidgetApi->getProperty("default")).toDouble(&isOk);
  if (isOk) {
    aBox->setValue(aDefVal);
  }
  aControlLay->addWidget(aBox);
  aControlLay->addStretch();

  result->setLayout(aControlLay);
  return result;
}

QString XGUI_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}

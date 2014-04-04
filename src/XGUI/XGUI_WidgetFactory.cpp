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
#include <QPixmap>

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
  aWidgetLay->setContentsMargins(0, 0, 0, 0);
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
  theParent->setLayout(aWidgetLay);
}

QWidget* XGUI_WidgetFactory::valueWidget()
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
  QString aTTip = qs(myWidgetApi->widgetTooltip());
  aBox->setToolTip(aTTip);
  aControlLay->addWidget(aBox);
  aControlLay->setStretch(1, 1);

  result->setLayout(aControlLay);
  return result;
}

QString XGUI_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}

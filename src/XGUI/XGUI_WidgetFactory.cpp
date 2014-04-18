/*
 * XGUI_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <XGUI_WidgetFactory.h>

#include <XGUI_SwitchWidget.h>

#include <ModuleBase_PropPanelOperation.h>
#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QMetaProperty>
#include <QLabel>
#include <QPixmap>
#include <QGroupBox>
#include <QToolBox>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <cfloat>
#include <climits>

XGUI_WidgetFactory::XGUI_WidgetFactory(ModuleBase_PropPanelOperation* theOperation)
    : myOperation(theOperation)
{
  QString aXml = myOperation->xmlRepresentation();
  myWidgetApi = new Config_WidgetAPI(aXml.toStdString());
}

XGUI_WidgetFactory::~XGUI_WidgetFactory()
{
}

void XGUI_WidgetFactory::createWidget(QWidget* theParent)
{
  if (!myWidgetApi->toChildWidget())
    return;

  QVBoxLayout* aWidgetLay = new QVBoxLayout(theParent);
  aWidgetLay->setContentsMargins(2, 2, 2, 2);
  do { //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    //Create a widget (doublevalue, groupbox, toolbox, etc.
    QWidget* aWidget = createWidgetByType(aWdgType, theParent);
    if (aWidget) {
      aWidgetLay->addWidget(aWidget);
    }
    if (myWidgetApi->isContainerWidget()) {
      //if current widget is groupbox (container) process it's children recursively
      QString aGroupName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
      createWidget(aWidget);
      QGroupBox* aGrBox = qobject_cast<QGroupBox*>(aWidget);
      aGrBox->setTitle(aGroupName);
    }
    if (myWidgetApi->isPagedWidget()) {
      //If current widget is toolbox or switch-casebox then fetch all
      //it's pages recursively and setup into the widget.
      myWidgetApi->toChildWidget();
      do {
        QString aPageName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
        QWidget* aPage = new QWidget(aWidget);
        createWidget(aPage);
        if (aWdgType == WDG_SWITCH) {
          XGUI_SwitchWidget* aSwitch = qobject_cast<XGUI_SwitchWidget*>(aWidget);
          aSwitch->addPage(aPage, aPageName);
        } else if (aWdgType == WDG_TOOLBOX){
          QToolBox* aToolbox = qobject_cast<QToolBox*>(aWidget);
          aToolbox->addItem(aPage, aPageName);
        }
      } while(myWidgetApi->toNextWidget());
    }
  } while(myWidgetApi->toNextWidget());
  theParent->setLayout(aWidgetLay);
}

QWidget* XGUI_WidgetFactory::createWidgetByType(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_DOUBLEVALUE) {
    result = doubleSpinBoxControl();
  } else if (myWidgetApi->isContainerWidget() || myWidgetApi->isPagedWidget()) {
    result = createContainer(theType, theParent);
  }
#ifdef _DEBUG
  else { qDebug() << "XGUI_WidgetFactory::fillWidget: find bad widget type"; }
#endif
  return result;
}

QWidget* XGUI_WidgetFactory::createContainer(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_GROUP || theType == WDG_CHECK_GROUP) {
    QGroupBox* aGroupBox = new QGroupBox(theParent);
    aGroupBox->setCheckable(theType == WDG_CHECK_GROUP);
    result = aGroupBox;
  } else if (theType == WDG_TOOLBOX) {
    result = new QToolBox(theParent);
  } else if (theType == WDG_SWITCH) {
    result = new XGUI_SwitchWidget(theParent);
  } else if (theType == WDG_TOOLBOX_BOX || theType == WDG_SWITCH_CASE) {
    result = NULL;
  }
#ifdef _DEBUG
  else { qDebug() << "XGUI_WidgetFactory::fillWidget: find bad container type"; }
#endif
  return result;
}

QWidget* XGUI_WidgetFactory::doubleSpinBoxControl()
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
  } else {
    aBox->setMinimum(-DBL_MAX);
  }
  aProp = myWidgetApi->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = qs(aProp).toDouble(&isOk);
  if (isOk) {
    aBox->setMaximum(aMaxVal);
  } else {
    aBox->setMaximum(DBL_MAX);
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
  connectWidget(aBox, WDG_DOUBLEVALUE);
  return result;
}

bool XGUI_WidgetFactory::connectWidget(QWidget* theWidget,  const QString& theType)
{
  bool result = false;
  if (theType == WDG_DOUBLEVALUE) {
    result = QObject::connect(theWidget, SIGNAL(valueChanged(double)), 
                              myOperation, SLOT(storeReal(double)));
  }
  return result;
}

QString XGUI_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}

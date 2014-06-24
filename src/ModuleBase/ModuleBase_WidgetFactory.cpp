/*
 * ModuleBase_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetFactory.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetFeature.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_WidgetSwitch.h>
#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_WidgetDoubleValue.h>
#include <ModuleBase_WidgetBoolValue.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
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

ModuleBase_WidgetFactory::ModuleBase_WidgetFactory(const std::string& theXmlRepresentation,
                                                   ModuleBase_IWorkshop* theWorkshop)
 : myWorkshop(theWorkshop)
{
  myWidgetApi = new Config_WidgetAPI(theXmlRepresentation);
}

ModuleBase_WidgetFactory::~ModuleBase_WidgetFactory()
{
}

void ModuleBase_WidgetFactory::createWidget(QWidget* theParent)
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
          ModuleBase_WidgetSwitch* aSwitch = qobject_cast<ModuleBase_WidgetSwitch*>(aWidget);
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

QWidget* ModuleBase_WidgetFactory::labelControl(QWidget* theParent)
{
  QWidget* result = new QWidget(theParent);
  QVBoxLayout* aLabelLay = new QVBoxLayout(result);
  QLabel* aLabel = new QLabel(result);
  aLabel->setText(qs(myWidgetApi->getProperty(INFO_WDG_TEXT)));
  aLabel->setToolTip(qs(myWidgetApi->getProperty(INFO_WDG_TOOLTIP)));
  aLabelLay->addWidget(aLabel);
  aLabelLay->addStretch(1);
  result->setLayout(aLabelLay);
  return result;
}

QWidget* ModuleBase_WidgetFactory::createWidgetByType(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_DOUBLEVALUE) {
    result = doubleSpinBoxControl(theParent);

  } else if (theType == WDG_INFO) {
    result = labelControl(theParent);

  } else if (theType == WDG_SELECTOR) {
    result = selectorControl(theParent);

  } else if (theType == WDG_BOOLVALUE) {
    result = booleanControl(theParent);

  } else if (theType == WDG_POINT_SELECTOR) {
    result = pointSelectorControl(theParent);

  } else if (theType == WDG_FEATURE_SELECTOR) {
    result = featureSelectorControl(theParent);

  } else if (theType == WDG_DOUBLEVALUE_EDITOR) {
    result = doubleValueEditor(theParent);

  }
  else if (myWidgetApi->isContainerWidget() || myWidgetApi->isPagedWidget()) {
    result = createContainer(theType, theParent);
  }
#ifdef _DEBUG
  else { qDebug() << "ModuleBase_WidgetFactory::fillWidget: find bad widget type"; }
#endif
  return result;
}

QWidget* ModuleBase_WidgetFactory::createContainer(const std::string& theType, QWidget* theParent)
{
  QWidget* result = NULL;
  if (theType == WDG_GROUP || theType == WDG_CHECK_GROUP) {
    QGroupBox* aGroupBox = new QGroupBox(theParent);
    aGroupBox->setCheckable(theType == WDG_CHECK_GROUP);
    result = aGroupBox;
  } else if (theType == WDG_TOOLBOX) {
    result = new QToolBox(theParent);
  } else if (theType == WDG_SWITCH) {
    result = new ModuleBase_WidgetSwitch(theParent);
  } else if (theType == WDG_TOOLBOX_BOX || theType == WDG_SWITCH_CASE) {
    result = NULL;
  }
#ifdef _DEBUG
  else { qDebug() << "ModuleBase_WidgetFactory::fillWidget: find bad container type"; }
#endif
  return result;
}

QWidget* ModuleBase_WidgetFactory::doubleSpinBoxControl(QWidget* theParent)
{
  ModuleBase_WidgetDoubleValue* aDblWgt = new ModuleBase_WidgetDoubleValue(theParent, myWidgetApi);
  myModelWidgets.append(aDblWgt);

  return aDblWgt->getControl();
}

QWidget* ModuleBase_WidgetFactory::pointSelectorControl(QWidget* theParent)
{
  ModuleBase_WidgetPoint2D* aWidget = new ModuleBase_WidgetPoint2D(theParent, myWidgetApi);
  myModelWidgets.append(aWidget);
  return aWidget->getControl();
}

QWidget* ModuleBase_WidgetFactory::featureSelectorControl(QWidget* theParent)
{
  ModuleBase_WidgetFeature* aWidget = new ModuleBase_WidgetFeature(theParent, myWidgetApi);
  myModelWidgets.append(aWidget);
  return aWidget->getControl();
}

QWidget* ModuleBase_WidgetFactory::doubleValueEditor(QWidget* theParent)
{
  ModuleBase_WidgetEditor* aWidget = new ModuleBase_WidgetEditor(theParent, myWidgetApi);
  myModelWidgets.append(aWidget);
  return 0;
}

QString ModuleBase_WidgetFactory::qs(const std::string& theStdString) const
{
  return QString::fromStdString(theStdString);
}


QWidget* ModuleBase_WidgetFactory::selectorControl(QWidget* theParent)
{
  ModuleBase_WidgetSelector* aSelector = new ModuleBase_WidgetSelector(theParent, myWorkshop, myWidgetApi);
  myModelWidgets.append(aSelector);
  return aSelector->getControl();
}


QWidget* ModuleBase_WidgetFactory::booleanControl(QWidget* theParent)
{
  ModuleBase_WidgetBoolValue* aBoolWgt = new ModuleBase_WidgetBoolValue(theParent, myWidgetApi);
  myModelWidgets.append(aBoolWgt);

  return aBoolWgt->getControl();
}
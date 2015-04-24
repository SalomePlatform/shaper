// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetFactory.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetFactory.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_WidgetSwitch.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_WidgetDoubleValue.h>
#include <ModuleBase_WidgetIntValue.h>
#include <ModuleBase_WidgetBoolValue.h>
#include <ModuleBase_WidgetFileSelector.h>
#include <ModuleBase_WidgetChoice.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetLineEdit.h>
#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_WidgetLabel.h>
#include <ModuleBase_WidgetToolbox.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_PageGroupBox.h>
#include <ModuleBase_PageWidget.h>
#include <ModuleBase_WidgetExprEditor.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

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
  delete myWidgetApi;
}

void ModuleBase_WidgetFactory::createWidget(ModuleBase_PageBase* thePage)
{
  myParentId = myWidgetApi->widgetId();
  if (!myWidgetApi->toChildWidget())
    return;

  do {  //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    // Create PageGroup TODO: extract
    if (myWidgetApi->isGroupBoxWidget()) {
      //if current widget is groupbox (container) process it's children recursively
      QString aGroupName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
      ModuleBase_PageGroupBox* aPage = new ModuleBase_PageGroupBox(thePage->pageWidget());
      aPage->setTitle(aGroupName);
      createWidget(aPage);
      thePage->addPageWidget(aPage);
    } else {
      // Create a ModelWidget
      ModuleBase_ModelWidget* aWidget = createWidgetByType(aWdgType, thePage->pageWidget());
      if (aWidget) {
        if (!myWidgetApi->getBooleanAttribute(ATTR_INTERNAL, false)) {
          thePage->addModelWidget(aWidget);
        } else {
          aWidget->setVisible(false);
        }
      }
      // Create PagedContainer TODO: extract
      if (myWidgetApi->isPagedWidget()) {
        //If current widget is toolbox or switch-casebox then fetch all
        //it's pages recursively and setup into the widget.
        myWidgetApi->toChildWidget();
        do {
          QString aPageName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
          QString aCaseId = qs(myWidgetApi->getProperty(_ID));
          ModuleBase_PageBase* aPage = new ModuleBase_PageWidget(aWidget);
          createWidget(aPage);
          if (aWdgType == WDG_SWITCH || aWdgType == WDG_TOOLBOX) {
            ModuleBase_PagedContainer* aContainer = qobject_cast<ModuleBase_PagedContainer*>(aWidget);
            aContainer->addPage(aPage, aPageName, aCaseId);
          }
        } while (myWidgetApi->toNextWidget());
      }
    }
  } while (myWidgetApi->toNextWidget());

  thePage->alignToTop();
}

ModuleBase_ModelWidget* ModuleBase_WidgetFactory::createWidgetByType(const std::string& theType,
                                                                     QWidget* theParent)
{
  ModuleBase_ModelWidget* result = NULL;

  if (theType == WDG_INFO) {
    result = new ModuleBase_WidgetLabel(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_DOUBLEVALUE) {
    result = new ModuleBase_WidgetDoubleValue(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_INTEGERVALUE) {
    result = new ModuleBase_WidgetIntValue(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_SHAPE_SELECTOR) {
    result = new ModuleBase_WidgetShapeSelector(theParent, myWorkshop, myWidgetApi, myParentId);
  } else if (theType == WDG_BOOLVALUE) {
    result = new ModuleBase_WidgetBoolValue(theParent, myWidgetApi, myParentId);
  //} else if (theType == WDG_DOUBLEVALUE_EDITOR) {
  //  result = new ModuleBase_WidgetEditor(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_FILE_SELECTOR) {
    result = new ModuleBase_WidgetFileSelector(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_CHOICE) {
    result = new ModuleBase_WidgetChoice(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_STRINGVALUE) {
    result = new ModuleBase_WidgetLineEdit(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_EXPR_EDITOR) {
    result = new ModuleBase_WidgetExprEditor(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_MULTISELECTOR) {
    result = new ModuleBase_WidgetMultiSelector(theParent, myWorkshop, myWidgetApi, myParentId);
  } else if (theType == WDG_TOOLBOX) {
    result = new ModuleBase_WidgetToolbox(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_SWITCH) {
    result = new ModuleBase_WidgetSwitch(theParent, myWidgetApi, myParentId);
  } else if (theType == WDG_TOOLBOX_BOX || theType == WDG_SWITCH_CASE) {
    // Do nothing for "box" and "case"
    result = NULL;
  } else {
    result = myWorkshop->module()->createWidgetByType(theType, theParent, myWidgetApi, myParentId);
    #ifdef _DEBUG
    if (!result) {
      qDebug("ModuleBase_WidgetFactory::fillWidget: find bad widget type");
    }
    #endif
  }
  if (result) {
    myModelWidgets.append(result);
  }
  return result;
}

QString ModuleBase_WidgetFactory::qs(const std::string& theStdString)
{
  return QString::fromStdString(theStdString);
}


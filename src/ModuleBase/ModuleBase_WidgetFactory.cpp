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
#include <ModuleBase_WidgetErrorLabel.h>
#include <ModuleBase_WidgetToolbox.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_PageGroupBox.h>
#include <ModuleBase_WidgetCheckGroupBox.h>
#include <ModuleBase_PageWidget.h>
#include <ModuleBase_WidgetExprEditor.h>
#include <ModuleBase_WidgetCreatorFactory.h>

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
  std::string aWType = myWidgetApi->widgetType();
  if (aWType == NODE_FEATURE) {
    QWidget* aPanel = createPanel(thePage->pageWidget());
    if (aPanel) {
      thePage->addWidget(aPanel);
      return;
    }
  }

  if (!myWidgetApi->toChildWidget())
    return;

  do {  //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    // Create PageGroup TODO: extract
    if (myWidgetApi->isGroupBoxWidget() ||
        ModuleBase_WidgetCreatorFactory::get()->hasPageWidget(aWdgType)) {

      //if current widget is groupbox (container) process it's children recursively
      ModuleBase_PageBase* aPage = createPageByType(aWdgType, thePage->pageWidget());

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

            QString anIconPath = qs( myWidgetApi->getProperty( CONTAINER_PAGE_ICON ) );
            QPixmap anIcon( anIconPath );
            aContainer->addPage( aPage, aPageName, aCaseId, anIcon );
          }
        } while (myWidgetApi->toNextWidget());
      }
    }
  } while (myWidgetApi->toNextWidget());

  thePage->alignToTop();
}

void ModuleBase_WidgetFactory::createWidget(ModuleBase_PageBase* thePage,
                                            const std::string& theWidgetId)
{
  bool aFound = false;
  moveToWidgetId(theWidgetId, aFound);
  if (aFound) {
    std::string aWdgType = myWidgetApi->widgetType();

    // Create a ModelWidget
    ModuleBase_ModelWidget* aWidget = createWidgetByType(aWdgType, thePage->pageWidget());
    if (aWidget) {
      if (!myWidgetApi->getBooleanAttribute(ATTR_INTERNAL, false)) {
        thePage->addModelWidget(aWidget);
      }
      else {
        aWidget->setVisible(false);
      }
    }
  }
  thePage->alignToTop();
}

void ModuleBase_WidgetFactory::getAttributeTitle(const std::string& theAttributeId,
                                                 std::string& theTitle)
{
  bool aFound = false;
  moveToWidgetId(theAttributeId, aFound);
  if (aFound) {
    theTitle = QString::fromStdString(myWidgetApi->widgetLabel()).toStdString().c_str();
    if (theTitle.empty())
      theTitle = QString::fromStdString(myWidgetApi->getProperty(CONTAINER_PAGE_NAME)).toStdString().c_str();
  }
}

void ModuleBase_WidgetFactory::getGreedAttribute(std::string& theAttributeId)
{
  if (!theAttributeId.empty())
    return;

  if (!myWidgetApi->toChildWidget())
    return;

  do {  //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    // Find title under PageGroup
    if (myWidgetApi->isGroupBoxWidget() ||
      ModuleBase_WidgetCreatorFactory::get()->hasPageWidget(aWdgType)) {
      getGreedAttribute(theAttributeId);
    }
    else {
      // Find title here
      std::string anAttributeId = myWidgetApi->widgetId();
      if (myWidgetApi->getBooleanAttribute(ATTR_GREED, false))
        theAttributeId = anAttributeId;
      if (theAttributeId.empty() && myWidgetApi->isPagedWidget()) {
        //If current widget is toolbox or switch-casebox then fetch all
        //it's pages recursively and setup into the widget.
        myWidgetApi->toChildWidget();
        do {
          getGreedAttribute(theAttributeId);
        } while (theAttributeId.empty() && myWidgetApi->toNextWidget());
      }
    }
  } while (theAttributeId.empty() && myWidgetApi->toNextWidget());
}

void ModuleBase_WidgetFactory::moveToWidgetId(const std::string& theWidgetId, bool& theFound)
{
  if (theFound)
    return;

  if (!myWidgetApi->toChildWidget())
    return;

  do {  //Iterate over each node
    std::string aWdgType = myWidgetApi->widgetType();
    // Find title under PageGroup
    if (myWidgetApi->isGroupBoxWidget() ||
      ModuleBase_WidgetCreatorFactory::get()->hasPageWidget(aWdgType)) {
      moveToWidgetId(theWidgetId, theFound);
    }
    else {
      // Find title here
      std::string anAttributeId = myWidgetApi->widgetId();
      theFound = anAttributeId == theWidgetId;
      if (!theFound && myWidgetApi->isPagedWidget()) {
        //If current widget is toolbox or switch-casebox then fetch all
        //it's pages recursively and setup into the widget.
        myWidgetApi->toChildWidget();
        do {
          moveToWidgetId(theWidgetId, theFound);
        } while (!theFound && myWidgetApi->toNextWidget());
      }
    }
  } while (!theFound && myWidgetApi->toNextWidget());
}

QWidget* ModuleBase_WidgetFactory::createPanel(QWidget* theParent)
{
  QWidget* aPanel = 0;
  std::string aPanelName = myWidgetApi->getProperty(PROPERTY_PANEL_ID);
  if (!aPanelName.empty() && ModuleBase_WidgetCreatorFactory::get()->hasPanelWidget(aPanelName))
    aPanel = ModuleBase_WidgetCreatorFactory::get()->createPanel(aPanelName, theParent);
  return aPanel;
}

ModuleBase_PageBase* ModuleBase_WidgetFactory::createPageByType(const std::string& theType,
                                                                QWidget* theParent)
{
  ModuleBase_PageBase* aResult = NULL;

  if (theType == WDG_GROUP) {
    QString aGroupName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
    ModuleBase_PageGroupBox* aPage = new ModuleBase_PageGroupBox(theParent);
    aPage->setTitle(aGroupName);
    aResult = aPage;
  }
  else if (theType == WDG_CHECK_GROUP) {
    QString aGroupName = qs(myWidgetApi->getProperty(CONTAINER_PAGE_NAME));
    ModuleBase_WidgetCheckGroupBox* aPage = new ModuleBase_WidgetCheckGroupBox(theParent,
                                                                myWidgetApi);
    aPage->setTitle(aGroupName);
    aResult = aPage;
  }
  if (!aResult)
    aResult = ModuleBase_WidgetCreatorFactory::get()->createPageByType(theType, theParent,
                                                                       myWidgetApi);

  ModuleBase_ModelWidget* aWidget = dynamic_cast<ModuleBase_ModelWidget*>(aResult);
  if (aWidget)
    myModelWidgets.append(aWidget);

  return aResult;
}

ModuleBase_ModelWidget* ModuleBase_WidgetFactory::createWidgetByType(const std::string& theType,
                                                                     QWidget* theParent)
{
  ModuleBase_ModelWidget* result = NULL;

  if (theType == WDG_INFO) {
    result = new ModuleBase_WidgetLabel(theParent, myWidgetApi);
  } else if (theType == WDG_ERRORINFO) {
    result = new ModuleBase_WidgetErrorLabel(theParent, myWidgetApi);
  } else if (theType == WDG_DOUBLEVALUE) {
    result = new ModuleBase_WidgetDoubleValue(theParent, myWidgetApi);
  } else if (theType == WDG_INTEGERVALUE) {
    result = new ModuleBase_WidgetIntValue(theParent, myWidgetApi);
  } else if (theType == WDG_SHAPE_SELECTOR) {
    result = new ModuleBase_WidgetShapeSelector(theParent, myWorkshop, myWidgetApi);
  } else if (theType == WDG_BOOLVALUE) {
    result = new ModuleBase_WidgetBoolValue(theParent, myWidgetApi);
  //} else if (theType == WDG_DOUBLEVALUE_EDITOR) {
  //  result = new ModuleBase_WidgetEditor(theParent, myWidgetApi);
  } else if (theType == WDG_FILE_SELECTOR) {
    result = new ModuleBase_WidgetFileSelector(theParent, myWidgetApi);
  } else if (theType == WDG_CHOICE) {
    result = new ModuleBase_WidgetChoice(theParent, myWidgetApi);
  } else if (theType == WDG_STRINGVALUE) {
    std::string aPlaceHolder = myWidgetApi->getProperty( WDG_PLACE_HOLDER );
    result = new ModuleBase_WidgetLineEdit( theParent, myWidgetApi, aPlaceHolder );
  } else if (theType == WDG_EXPR_EDITOR) {
    std::string aPlaceHolder = myWidgetApi->getProperty( WDG_PLACE_HOLDER );
    result = new ModuleBase_WidgetExprEditor( theParent, myWidgetApi, aPlaceHolder );
  } else if (theType == WDG_MULTISELECTOR) {
    result = new ModuleBase_WidgetMultiSelector(theParent, myWorkshop, myWidgetApi);
  } else if (theType == WDG_TOOLBOX) {
    result = new ModuleBase_WidgetToolbox(theParent, myWidgetApi);
  } else if (theType == WDG_SWITCH) {
    result = new ModuleBase_WidgetSwitch(theParent, myWidgetApi);
  } else if (theType == WDG_TOOLBOX_BOX || theType == WDG_SWITCH_CASE ||
             theType == NODE_VALIDATOR) {
    // Do nothing for "box" and "case"
    result = NULL;
  } else {
    result = myWorkshop->module()->createWidgetByType(theType, theParent, myWidgetApi);
    if (!result)
      result = ModuleBase_WidgetCreatorFactory::get()->createWidgetByType(theType, theParent,
                                                              myWidgetApi, myWorkshop);
    #ifdef _DEBUG
    if (!result) {
      qDebug("ModuleBase_WidgetFactory::fillWidget: find bad widget type %s", theType.c_str());
    }
    #endif
  }
  if (result)
    myModelWidgets.append(result);
  return result;
}

QString ModuleBase_WidgetFactory::qs(const std::string& theStdString)
{
  return QString::fromStdString(theStdString);
}


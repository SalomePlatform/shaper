/*
 * ModuleBase_PageBase.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QLayout>

class QWidget;

ModuleBase_PageBase::ModuleBase_PageBase()
{

}

ModuleBase_PageBase::~ModuleBase_PageBase()
{

}

QWidget* ModuleBase_PageBase::pageWidget()
{
  return dynamic_cast<QWidget*>(this);
}

void ModuleBase_PageBase::addModelWidget(ModuleBase_ModelWidget* theWidget)
{
  placeModelWidget(theWidget);
  myWidgetList.append(theWidget);
}

void ModuleBase_PageBase::addPageWidget(ModuleBase_PageBase* thePage)
{
  placePageWidget(thePage);
}

void ModuleBase_PageBase::clearPage()
{
  QLayoutItem *aChild;
  while ((aChild = pageLayout()->takeAt(0)) != 0) {
    if(aChild->widget()) {
      aChild->widget()->deleteLater();
    } else {
      delete aChild;
    }
  }
  myWidgetList.clear();
}


bool ModuleBase_PageBase::takeFocus()
{
  if(myWidgetList.isEmpty())
    return false;

  return myWidgetList.first()->focusTo();
}

QList<ModuleBase_ModelWidget*> ModuleBase_PageBase::modelWidgets()
{
  return myWidgetList;
}

void ModuleBase_PageBase::alignToTop()
{
  bool hasExpanding = false;
  QList<QWidget *> aListToCheck;
  ModuleBase_ModelWidget* aModelWidget;
  foreach(aModelWidget, myWidgetList) {
    aListToCheck << aModelWidget->getControls();
  }
  foreach(QWidget* eachWidget, aListToCheck) {
    QSizePolicy::Policy aVPolicy = eachWidget->sizePolicy().verticalPolicy();
    if(aVPolicy & QSizePolicy::ExpandFlag) {
      hasExpanding = true;
      break;
    }
  }
  if(!hasExpanding) {
    addPageStretch();
  }
}

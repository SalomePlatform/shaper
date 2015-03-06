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
  qDeleteAll(pageLayout()->children());
  myWidgetList.clear();
}


void ModuleBase_PageBase::takeFocus()
{
  if(myWidgetList.isEmpty())
    return;

  myWidgetList.first()->focusTo();
}

QList<ModuleBase_ModelWidget*> ModuleBase_PageBase::modelWidgets()
{
  return myWidgetList;
}

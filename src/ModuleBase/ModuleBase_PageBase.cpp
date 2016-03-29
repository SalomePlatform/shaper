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

void ModuleBase_PageBase::addWidget(QWidget* theWidget)
{
  placeWidget(theWidget);
}

void ModuleBase_PageBase::clearPage()
{
  myWidgetList.clear();

  QLayoutItem *aChild;
  while ((aChild = pageLayout()->takeAt(0)) != 0) {
    if(aChild->widget()) {
      delete aChild->widget();
    } else {
      delete aChild;
    }
  }

  // Issue #460: this patch is necessary since the row stretch information
  // is stored independently on the items: when the items are removed
  // from the layout the stretch information is kept, so in the next 
  // filling of the layout some "additional" (old) rows may be stretched
  // without necessity. 
  // In this patch we clear the stretch information specifying the default value: 0.
  QGridLayout* aLayout = dynamic_cast<QGridLayout*>( pageLayout() );
  if( aLayout )
  {
    int r = aLayout->rowCount();
    for( int i=0; i<r; i++ )
      aLayout->setRowStretch( i, 0 );
  }
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

void ModuleBase_PageBase::placePageWidget(ModuleBase_PageBase* theWidget)
{
  QWidget* aWidget = dynamic_cast<QWidget*>(theWidget);
  placeWidget(aWidget);
}

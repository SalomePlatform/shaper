// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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

QList<ModuleBase_ModelWidget*> ModuleBase_PageBase::modelWidgets() const
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

// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ModuleBase_WidgetToolbox.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ToolBox.h>

#include <ModelAPI_AttributeString.h>

#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QIcon>

ModuleBase_WidgetToolbox::ModuleBase_WidgetToolbox(QWidget* theParent,
                                                   const Config_WidgetAPI* theData)
: ModuleBase_PagedContainer(theParent, theData)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aMainLayout);

  bool aHasContainerParent = false;
  QWidget* aParent = dynamic_cast<QWidget*>(parent());
  while(aParent && !aHasContainerParent) {
    ModuleBase_PagedContainer* aPagedContainer = dynamic_cast<ModuleBase_PagedContainer*>(aParent);
    aHasContainerParent = aPagedContainer;
    aParent = dynamic_cast<QWidget*>(aParent->parent());
  }
  myToolBox = new ModuleBase_ToolBox(this, aHasContainerParent);
  // Dark-grey rounded tabs with button-like border #and bold font
  // TODO: apply style to custom widget
  QString css = "QToolBox::tab{background-color:#c8c8c8;"
                              "border-radius:5px;"
                              "border:1px inset;"
                              //"font-weight:700;"
                              "border-color:#fff #505050 #505050 #fff;}";
  myToolBox->setStyleSheet(css);
  // default vertical size policy is preferred
  aMainLayout->addWidget(myToolBox);

  connect(myToolBox, SIGNAL(currentChanged(int)), this, SLOT(onPageChanged()));
}

ModuleBase_WidgetToolbox::~ModuleBase_WidgetToolbox()
{
}

int ModuleBase_WidgetToolbox::addPage(ModuleBase_PageBase* thePage,
                                      const QString& theName,
                                      const QString& theCaseId,
                                      const QPixmap& theIcon,
                                      const QString& theTooltip)
{
  ModuleBase_PagedContainer::addPage(thePage, theName, theCaseId, theIcon, theTooltip);
  QFrame* aFrame = dynamic_cast<QFrame*>(thePage);
  myToolBox->addItem(aFrame, translate(theName.toStdString()), theIcon );
  return myToolBox->count();
}

int ModuleBase_WidgetToolbox::currentPageIndex() const
{
  return myToolBox->currentIndex();
}

void ModuleBase_WidgetToolbox::setCurrentPageIndex(int theIndex)
{
  bool isSignalsBlocked = myToolBox->blockSignals(true);
  myToolBox->setCurrentIndex(theIndex);
  myToolBox->blockSignals(isSignalsBlocked);
}



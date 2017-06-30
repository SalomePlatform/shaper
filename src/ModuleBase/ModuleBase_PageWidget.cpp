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

#include <ModuleBase_PageWidget.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <QLayout>

#include <iostream>

ModuleBase_PageWidget::ModuleBase_PageWidget(QWidget* theParent)
: QFrame(theParent)
{
  myMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);
  setLayout(myMainLayout);
}

ModuleBase_PageWidget::~ModuleBase_PageWidget()
{
}

void ModuleBase_PageWidget::addPageStretch()
{
  myMainLayout->addStretch(1);
}

void ModuleBase_PageWidget::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  myMainLayout->addWidget(theWidget, 0);
}

void ModuleBase_PageWidget::placeWidget(QWidget* theWidget)
{
  if (!theWidget) {
    #ifdef _DEBUG
    std::cout << "ModuleBase_PageWidget::placePageWidget: can not cast page" << std::endl;
    #endif
    return;
  }
  myMainLayout->addWidget(theWidget, 0);
}

QLayout* ModuleBase_PageWidget::pageLayout()
{
  return myMainLayout;
}

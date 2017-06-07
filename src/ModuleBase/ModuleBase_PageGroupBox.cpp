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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

/*
 * ModuleBase_PageGroupBox.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#include <ModuleBase_PageGroupBox.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <QGridLayout>

ModuleBase_PageGroupBox::ModuleBase_PageGroupBox(QWidget* theParent)
: QGroupBox(theParent)
{
  myMainLayout = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);
  setLayout(myMainLayout);
}

ModuleBase_PageGroupBox::~ModuleBase_PageGroupBox()
{
}

void ModuleBase_PageGroupBox::addPageStretch()
{
}

void ModuleBase_PageGroupBox::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  // it seems, that the align on left is not necessary here, but leads to widgets, which are
  // not extended on full width of the parent page. The case is grouped widgets in
  // the sketch translation operation
  myMainLayout->addWidget(theWidget, kRow, kCol, Qt::AlignTop);// | Qt::AlignLeft);
  myMainLayout->setRowStretch(kRow, 0);

}

void ModuleBase_PageGroupBox::placeWidget(QWidget* theWidget)
{
  if (!theWidget) {
    #ifdef _DEBUG
    std::cout << "ModuleBase_PageGroupBox::placePageWidget: can not cast page" << std::endl;
    #endif
    return;
  }
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  myMainLayout->addWidget(theWidget, kRow, kCol);
  myMainLayout->setRowStretch(kRow, 0);
}

QLayout* ModuleBase_PageGroupBox::pageLayout()
{
  return myMainLayout;
}

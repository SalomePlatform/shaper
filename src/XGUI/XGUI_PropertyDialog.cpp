// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <XGUI_PropertyDialog.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>

XGUI_PropertyDialog::XGUI_PropertyDialog(QWidget* theParent)
: QDialog(theParent), myContentWidget(0)
{
  myLayout = new QGridLayout(this);

  QDialogButtonBox* aButtons = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));

  myLayout->addWidget(aButtons, 1, 0);
}

void XGUI_PropertyDialog::setContent(QWidget* theWidget)
{
  myLayout->addWidget(theWidget, 0, 0);
}

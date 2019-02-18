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

#include <XGUI_DeflectionDialog.h>

#include <ModelAPI_Tools.h>

#include <ModuleBase_DoubleSpinBox.h>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_DeflectionDialog::XGUI_DeflectionDialog(QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle(tr("Deflection"));
  QGridLayout* aLay = new QGridLayout(this);

  myDeflection = new ModuleBase_DoubleSpinBox(this);
  myDeflection->setDecimals(12);
  myDeflection->setRange(0, 1);
  myDeflection->setSingleStep(0.001);
  aLay->addWidget(myDeflection, 0, 0);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 1, 0);
}

void XGUI_DeflectionDialog::setDeflection(const double& theValue)
{
  myDeflection->setValue(theValue);
}

double XGUI_DeflectionDialog::getDeflection() const
{
  return myDeflection->value();
}

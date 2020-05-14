// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <ModuleBase_LabelValue.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_Tools.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>

ModuleBase_LabelValue::ModuleBase_LabelValue(QWidget* theParent, const QString& theText,
                                             const QString& theToolTip, const QString& theIcon,
                                             int thePrecision)
: QWidget(theParent), myValue(0), myPrecision(thePrecision)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(2, 0, 0, 0);
  aLayout->setSpacing(0);

  myLabel = new QLabel(QString("%1 : ").arg(theText), this);
  if (!theIcon.isEmpty()) {
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(theIcon));
    aLayout->setSpacing(4);
  }
  myLabel->setToolTip(!theToolTip.isEmpty() ? theToolTip : theText);
  aLayout->addWidget(myLabel);

  myLabelValue = new QLabel("", this);
  aLayout->addWidget(myLabelValue, 1);

  setLocale(ModuleBase_Tools::doubleLocale());
  aLayout->addStretch(1);
}

ModuleBase_LabelValue::~ModuleBase_LabelValue()
{
}

void ModuleBase_LabelValue::setValue(const double theValue)
{
  myValue = theValue;

  QString aStrValue = locale().toString(theValue, myPrecision >= 0 ? 'f' : 'g', qAbs(myPrecision));
  myLabelValue->setText(aStrValue);
  myLabelValue->setToolTip(aStrValue);
}

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

#include "XGUI_TransparencyWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

XGUI_TransparencyWidget::XGUI_TransparencyWidget(QWidget* theParent, const QString& theLabelText)
  : QWidget(theParent)
{
  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setContentsMargins(0, 0, 0, 0);

  QWidget* aInfoWgt = new QWidget(this);
  QHBoxLayout* aInfoLay = new QHBoxLayout(aInfoWgt);
  aInfoLay->setContentsMargins(0, 0, 0, 0);

  aInfoLay->addWidget(new QLabel(tr("Opaque")));

  myValLbl = new QLabel("0%", aInfoWgt);
  myValLbl->setAlignment(Qt::AlignCenter);
  aInfoLay->addWidget(myValLbl, 1);

  aInfoLay->addWidget(new QLabel(tr("Transparent")));
  aLay->addWidget(aInfoWgt);

  mySliderValue = new QSlider(Qt::Horizontal, this);
  mySliderValue->setRange(0, 100);
  aLay->addWidget(mySliderValue);

  connect(mySliderValue, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
}

void XGUI_TransparencyWidget::setValue(double theValue)
{
  bool isSliderBlocked = mySliderValue->blockSignals(true);
  int aVal = theValue * 100;
  mySliderValue->setValue(aVal);
  myValLbl->setText(QString("%1%").arg(aVal));
  mySliderValue->blockSignals(isSliderBlocked);
}

double XGUI_TransparencyWidget::getValue() const
{
  return mySliderValue->value() / 100.;
}

void XGUI_TransparencyWidget::onSliderValueChanged(int theValue)
{
  myValLbl->setText(QString("%1%").arg(theValue));
  emit transparencyValueChanged();
}

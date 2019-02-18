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

#include "XGUI_TransparencyWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

XGUI_TransparencyWidget::XGUI_TransparencyWidget(QWidget* theParent, const QString& theLabelText)
  : QWidget(theParent)
{
  QHBoxLayout* aLay = new QHBoxLayout(this);
  aLay->setContentsMargins(0, 0, 0, 0);

  mySpinValue = new QDoubleSpinBox(this);
  mySpinValue->setRange(0, 1);
  mySpinValue->setSingleStep(0.1);
  mySliderValue = new QSlider(Qt::Horizontal, this);
  mySliderValue->setRange(0, 100);

  myPreview = new QCheckBox("Preview", this);
  myPreview->setChecked(true);

  if (!theLabelText.isEmpty())
    aLay->addWidget(new QLabel(theLabelText, this));
  aLay->addWidget(mySpinValue);
  aLay->addWidget(mySliderValue);
  aLay->addWidget(myPreview);

  connect(mySpinValue, SIGNAL(valueChanged(double)), this, SLOT(onSpinValueChanged(double)));
  connect(mySliderValue, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
  connect(myPreview, SIGNAL(toggled(bool)), this, SIGNAL(previewStateChanged()));
}

void XGUI_TransparencyWidget::setValue(double theValue)
{
  bool isSpinBlocked = mySpinValue->blockSignals(true);
  bool isSliderBlocked = mySliderValue->blockSignals(true);

  mySpinValue->setValue(theValue);
  mySliderValue->setValue(theValue * 100);

  mySpinValue->blockSignals(isSpinBlocked);
  mySliderValue->blockSignals(isSliderBlocked);
}

double XGUI_TransparencyWidget::getValue() const
{
  return mySpinValue->value();
}

bool XGUI_TransparencyWidget::isPreviewNeeded() const
{
  return myPreview->isChecked();
}

void XGUI_TransparencyWidget::onSpinValueChanged(double theValue)
{
  setValue(theValue);
  emit transparencyValueChanged();
}

void XGUI_TransparencyWidget::onSliderValueChanged(int theValue)
{
  setValue((double)theValue / 100);
  emit transparencyValueChanged();
}

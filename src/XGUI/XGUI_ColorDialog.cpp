// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ColorDialog.cpp
// Created:     27 Apr 2015
// Author:      Natalia ERMOLAEVA

#include <XGUI_ColorDialog.h>

#include <ModelAPI_Tools.h>

#include <QtxColorButton.h>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_ColorDialog::XGUI_ColorDialog(QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle("Color");
  QGridLayout* aLay = new QGridLayout(this);

  QRadioButton* aRandomChoiceBtn = new QRadioButton(this);
  QRadioButton* aColorChoiceBtn = new QRadioButton(this);
  aColorChoiceBtn->setChecked(true);
  myButtonGroup = new QButtonGroup(this);
  myButtonGroup->setExclusive(true);
  myButtonGroup->addButton(aColorChoiceBtn, 0);
  myButtonGroup->addButton(aRandomChoiceBtn, 1);

  aLay->addWidget(aColorChoiceBtn, 0, 0);
  aLay->addWidget(aRandomChoiceBtn, 1, 0);

  myColorButton = new QtxColorButton(this);
  myColorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  aLay->addWidget(myColorButton, 0, 1);

  QLabel* aRandomLabel = new QLabel("Random", this);
  aLay->addWidget(aRandomLabel, 1, 1);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 2, 0, 1, 2);
}

bool XGUI_ColorDialog::isRandomColor() const
{
  int anId = myButtonGroup->checkedId();

  return myButtonGroup->checkedId() == 1;
}

void XGUI_ColorDialog::setColor(const std::vector<int>& theValue)
{
  if (theValue.size() != 3)
    return;

  myColorButton->setColor(QColor(theValue[0], theValue[1], theValue[2]));
}

std::vector<int> XGUI_ColorDialog::getColor() const
{
  std::vector<int> aValues;
  if (isRandomColor()) {
    ModelAPI_Tools::findRandomColor(aValues);
  }
  else {
    QColor aColorResult = myColorButton->color();
    aValues.push_back(aColorResult.red());
    aValues.push_back(aColorResult.green());
    aValues.push_back(aColorResult.blue());
  }
  return aValues;
}

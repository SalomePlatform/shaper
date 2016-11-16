// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetField.cpp
// Created:     16 Nov 2016
// Author:      Vitaly SMETANNIKOV

#include "CollectionPlugin_WidgetField.h"

#include <QLayout>
#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>

CollectionPlugin_WidgetField::
  CollectionPlugin_WidgetField(QWidget* theParent, const Config_WidgetAPI* theData):
ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);

  // Types definition controls
  QWidget* aTypesWgt = new QWidget(this);
  QFormLayout* aTypesLayout = new QFormLayout(aTypesWgt);
  aTypesLayout->setContentsMargins(0, 0, 0, 0);
  aMainLayout->addWidget(aTypesWgt);

  // Type of shapes
  myShapeTypeCombo = new QComboBox(aTypesWgt);
  QStringList aShapeTypes;
  aShapeTypes << tr("Vertices") << tr("Edges") << tr("Faces") 
    << tr("Solids") << tr("Results") << tr("Parts");
  myShapeTypeCombo->addItems(aShapeTypes);
  aTypesLayout->addRow(tr("Type of shapes"), myShapeTypeCombo);

  // Type of field
  myFieldTypeCombo = new QComboBox(aTypesWgt);
  QStringList aFieldTypes;
  aFieldTypes << tr("Double") << tr("Integer") << tr("Boolean") 
    << tr("String");
  myFieldTypeCombo->addItems(aFieldTypes);
  aTypesLayout->addRow(tr("Type of field"), myFieldTypeCombo);

  // Number of components
  myNbComponentsSpn = new QSpinBox(aTypesWgt);
  myNbComponentsSpn->setMinimum(1);
  aTypesLayout->addRow(tr("Nb. of components"), myNbComponentsSpn);

  // Steps controls
  QFrame* aStepFrame = new QFrame(this);
  aStepFrame->setFrameShape(QFrame::Box);
  aStepFrame->setFrameStyle(QFrame::StyledPanel);
  QGridLayout* aStepLayout = new QGridLayout(aStepFrame);
  aMainLayout->addWidget(aStepFrame);

  // Current step label
  aStepLayout->addWidget(new QLabel(tr("Current step"), aStepFrame), 0, 0);
  myCurStepLbl = new QLabel("1", aStepFrame);
  QFont aFont = myCurStepLbl->font();
  aFont.setBold(true);
  myCurStepLbl->setFont(aFont);
  aStepLayout->addWidget(myCurStepLbl, 0, 1);

  // Steps slider
  QWidget* aSliderWidget = new QWidget(aStepFrame);
  aStepLayout->addWidget(aSliderWidget, 1, 0, 1, 2);
  QHBoxLayout* aSliderLayout = new QHBoxLayout(aSliderWidget);
  aSliderLayout->setContentsMargins(0, 0, 0, 0);

  aSliderLayout->addWidget(new QLabel("1", aSliderWidget));

  myStepSlider = new QSlider(Qt::Horizontal, aSliderWidget);
  myStepSlider->setTickPosition(QSlider::TicksBelow);
  myStepSlider->setRange(1, 1);
  myStepSlider->setPageStep(myStepSlider->singleStep());
  aSliderLayout->addWidget(myStepSlider, 1);

  myMaxLbl = new QLabel("1", aSliderWidget);
  aSliderLayout->addWidget(myMaxLbl);

  // Stamp value
  aStepLayout->addWidget(new QLabel(tr("Stamp"), aStepFrame), 2, 0);
  myStampSpn = new QSpinBox(aStepFrame);
  aStepLayout->addWidget(myStampSpn, 2, 1);

  // Data table
  myDataTbl = new QTableWidget(2, 2, aStepFrame);
  myDataTbl->verticalHeader()->hide();
  myDataTbl->horizontalHeader()->hide();
  myDataTbl->setRowHeight(0, 25);
  myDataTbl->setRowHeight(1, 25);

  QTableWidgetItem* aItem = new QTableWidgetItem("Shape");
  aItem->setBackgroundColor(Qt::lightGray);
  aItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  myDataTbl->setItem(0, 0, aItem);

  aItem = new QTableWidgetItem("Comp 1");
  aItem->setBackgroundColor(Qt::lightGray);
  myDataTbl->setItem(0, 1, aItem);

  aItem = new QTableWidgetItem("Default value");
  aItem->setBackgroundColor(Qt::lightGray);
  aItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  myDataTbl->setItem(1, 0, aItem);

  aItem = new QTableWidgetItem("0");
  aItem->setBackgroundColor(Qt::lightGray);
  myDataTbl->setItem(1, 1, aItem);
  
  aStepLayout->addWidget(myDataTbl, 3, 0, 1, 2);

  // Buttons below
  QWidget* aBtnWgt = new QWidget(this);
  aMainLayout->addWidget(aBtnWgt);
  QHBoxLayout* aBtnLayout = new QHBoxLayout(aBtnWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);

  QPushButton* aAddBtn = new QPushButton(tr("Add step"), aBtnWgt);
  aBtnLayout->addWidget(aAddBtn);

  aBtnLayout->addStretch(1);

  QPushButton* aRemoveBtn = new QPushButton(tr("Remove step"), aBtnWgt);
  aBtnLayout->addWidget(aRemoveBtn);

  connect(myNbComponentsSpn, SIGNAL(valueChanged(int)), SLOT(onNbCompChanged(int)));
  connect(aAddBtn, SIGNAL(clicked(bool)), SLOT(onAddStep()));
  connect(aRemoveBtn, SIGNAL(clicked(bool)), SLOT(onRemoveStep()));
  connect(myStepSlider, SIGNAL(valueChanged(int)), SLOT(onStepMove(int)));
}

QList<QWidget*> CollectionPlugin_WidgetField::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  //aControls.push_back(myComboBox);
  return aControls;
}

bool CollectionPlugin_WidgetField::storeValueCustom()
{
  //AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  //AttributeIntegerPtr aValueAttribute =
  //                      std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  //aValueAttribute->setValue(myComboBox->currentIndex());
  //updateObject(myFeature);
  return true;
}

bool CollectionPlugin_WidgetField::restoreValueCustom()
{
  //AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  //AttributeIntegerPtr aValueAttribute =
  //                      std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);

  //bool isBlocked = myComboBox->blockSignals(true);
  //myComboBox->setCurrentIndex(aValueAttribute->value());
  //myComboBox->blockSignals(isBlocked);

  return true;
}

void CollectionPlugin_WidgetField::onNbCompChanged(int theVal)
{
  int aOldCol = myDataTbl->columnCount() - 1;
  int aNbRows = myDataTbl->rowCount();
  myDataTbl->setColumnCount(theVal + 1);
  int aDif = theVal - aOldCol;
  QTableWidgetItem* aItem = 0;
  for (int i = 0; i < aDif; i++) {
    for (int j = 0; j < aNbRows; j++) {
      aItem = new QTableWidgetItem();
      if (j == 0)
        aItem->setText(QString("Comp %1").arg(i + aOldCol + 1));
      else
        aItem->setText("0");
      if (j < 3)
        aItem->setBackgroundColor(Qt::lightGray);
      myDataTbl->setItem(j, i + aOldCol + 1, aItem);
    }
  }
}

void CollectionPlugin_WidgetField::onAddStep()
{
  int aMax = myStepSlider->maximum();
  aMax++;
  myStepSlider->setMaximum(aMax);
  myMaxLbl->setText(QString::number(aMax));
  clearData();
  myStepSlider->setValue(aMax);
}

void CollectionPlugin_WidgetField::onRemoveStep()
{
  int aMax = myStepSlider->maximum();
  aMax--;
  myStepSlider->setMaximum(aMax);
  myMaxLbl->setText(QString::number(aMax));
}

void CollectionPlugin_WidgetField::clearData()
{
  int aNbRows = myDataTbl->rowCount();
  int aNbCol = myDataTbl->columnCount();

  QString aDefValue;
  for (int i = 1; i < aNbCol; i++) {
    aDefValue = myDataTbl->item(1, i)->text();
    for (int j = 2; j < aNbRows; j++) {
      myDataTbl->item(j, i)->setText(aDefValue);
    }
  }
}

void CollectionPlugin_WidgetField::onStepMove(int theStep)
{
  myCurStepLbl->setText(QString::number(theStep));
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_WidgetTable.h"

#include <Config_WidgetAPI.h>
#include <ModuleBase_Tools.h>

#include <ModuleBase_TableModel.h>
#include <ModuleBase_Tools.h>
#include <QTableView>

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

ModuleBase_WidgetTable::ModuleBase_WidgetTable(QWidget* theParent,
                                               const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  QString aLabel = "Values";

  QStringList aHeaders;
  aHeaders << "X" << "Y" << "Z";

  QStringList aValues1, aValues2, aValues3;

  aValues1 << "0" << "0" << "0";
  aValues2 << "0.1" << "0.1" << "0.1";
  aValues3 << "0.2" << "0.2" << "0.2";

  QGridLayout* aLayout = new QGridLayout(this);

  myTableModel = new ModuleBase_TableModel(theParent);
  myTableModel->setHeaders(aHeaders);
  myTableModel->addRow(aValues1);
  myTableModel->addRow(aValues2);
  myTableModel->addRow(aValues3);

  myTableView = new QTableView(this);
  myTableView->setModel(myTableModel);

  QPushButton* anAddBtn = new QPushButton("+", this);
  QPushButton* aRemBtn = new QPushButton("-", this);

  connect(anAddBtn, SIGNAL(clicked()), this, SLOT(onAddRow()));

  aLayout->addWidget(new QLabel(aLabel), 0, 0);
  aLayout->addWidget(anAddBtn, 0, 1);
  aLayout->addWidget(aRemBtn, 0, 2);

  aLayout->addWidget(myTableView, 1, 0, 1, 3);

  //QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  //aLayout->addWidget(myLabel);
  setLayout(aLayout);
}

ModuleBase_WidgetTable::~ModuleBase_WidgetTable()
{
}

QList<QWidget*> ModuleBase_WidgetTable::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTableView);
  return aControls;
}

void ModuleBase_WidgetTable::onAddRow()
{
  myTableModel->addRow();
}

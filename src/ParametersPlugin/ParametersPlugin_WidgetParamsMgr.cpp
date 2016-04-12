// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_WidgetParamsMgr.cpp
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#include "ParametersPlugin_WidgetParamsMgr.h"

#include <QLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>

ParametersPlugin_WidgetParamsMgr::ParametersPlugin_WidgetParamsMgr(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);

  myTable = new QTreeWidget(this);
  myTable->setColumnCount(4);
  QStringList aHeaders;
  aHeaders << tr("Name") << tr("Equation") << tr("Result") << tr("Comment");
  myTable->setHeaderLabels(aHeaders);
  aLayout->addWidget(myTable);

  QHBoxLayout* aBtnLayout = new QHBoxLayout(this);

  QToolButton* aUpBtn = new QToolButton(this);
  aUpBtn->setArrowType(Qt::DownArrow);
  aBtnLayout->addWidget(aUpBtn);
  QToolButton* aDownBtn = new QToolButton(this);
  aDownBtn->setArrowType(Qt::UpArrow);
  aBtnLayout->addWidget(aDownBtn);

  aBtnLayout->addStretch();

  QPushButton* aAddBtn = new QPushButton(tr("Add"), this);
  aBtnLayout->addWidget(aAddBtn);
  QPushButton* aInsertBtn = new QPushButton(tr("Insert"), this);
  aBtnLayout->addWidget(aInsertBtn);
  QPushButton* aRemoveBtn = new QPushButton(tr("Remove"), this);
  aBtnLayout->addWidget(aRemoveBtn);

  aLayout->addLayout(aBtnLayout);
}

QList<QWidget*> ParametersPlugin_WidgetParamsMgr::getControls() const
{
  QList<QWidget*> aList;

  return aList;
}

bool ParametersPlugin_WidgetParamsMgr::storeValueCustom() const
{
  return true;
}

bool ParametersPlugin_WidgetParamsMgr::restoreValueCustom()
{
  return true;
}

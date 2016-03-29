/*
 * ModuleBase_PageWidget.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#include <ModuleBase_PageWidget.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <QGridLayout>

#include <iostream>

ModuleBase_PageWidget::ModuleBase_PageWidget(QWidget* theParent)
: QFrame(theParent)
{
  myMainLayout = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);
  setLayout(myMainLayout);
}

ModuleBase_PageWidget::~ModuleBase_PageWidget()
{
}

void ModuleBase_PageWidget::addPageStretch()
{
  myMainLayout->setRowStretch(myMainLayout->rowCount(), 1);
}

void ModuleBase_PageWidget::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  myMainLayout->addWidget(theWidget, kRow, kCol);
  myMainLayout->setRowStretch(kRow, 0);
}

void ModuleBase_PageWidget::placeWidget(QWidget* theWidget)
{
  if (!theWidget) {
    #ifdef _DEBUG
    std::cout << "ModuleBase_PageWidget::placePageWidget: can not cast page" << std::endl;
    #endif
    return;
  }
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  myMainLayout->addWidget(theWidget, kRow, kCol);
  myMainLayout->setRowStretch(kRow, 0);
}

QLayout* ModuleBase_PageWidget::pageLayout()
{
  return myMainLayout;
}

/*
 * ModuleBase_PageGroupBox.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#include <ModuleBase_PageGroupBox.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <QGridLayout>

ModuleBase_PageGroupBox::ModuleBase_PageGroupBox(QWidget* theParent)
: QGroupBox(theParent)
{
  myMainLayout = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);
  setLayout(myMainLayout);
}

ModuleBase_PageGroupBox::~ModuleBase_PageGroupBox()
{
}

void ModuleBase_PageGroupBox::addPageStretch()
{
}

void ModuleBase_PageGroupBox::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  // it seems, that the align on left is not necessary here, but leads to widgets, which are
  // not extended on full width of the parent page. The case is grouped widgets in
  // the sketch translation operation
  myMainLayout->addWidget(theWidget, kRow, kCol, Qt::AlignTop);// | Qt::AlignLeft);
  myMainLayout->setRowStretch(kRow, 0);

}

void ModuleBase_PageGroupBox::placeWidget(QWidget* theWidget)
{
  if (!theWidget) {
    #ifdef _DEBUG
    std::cout << "ModuleBase_PageGroupBox::placePageWidget: can not cast page" << std::endl;
    #endif
    return;
  }
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  myMainLayout->addWidget(theWidget, kRow, kCol);
  myMainLayout->setRowStretch(kRow, 0);
}

QLayout* ModuleBase_PageGroupBox::pageLayout()
{
  return myMainLayout;
}

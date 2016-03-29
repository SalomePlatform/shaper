// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_WidgetCheckGroupBox.cpp
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetCheckGroupBox.h>
#include <ModelAPI_AttributeBoolean.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include <QList>

ModuleBase_WidgetCheckGroupBox::ModuleBase_WidgetCheckGroupBox(QWidget* theParent,
                                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData),
  ModuleBase_PageBase()
{
  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  bool isChecked = theData->getBooleanAttribute(ATTR_DEFAULT, false);

  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aMainLayout);
  myGroupBox = new QGroupBox(this);
  myGroupBox->setCheckable(true);
  myGroupBox->setToolTip(aToolTip);
  myGroupBox->setChecked(isChecked);

  myMainLayout = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(myMainLayout);
  myGroupBox->setLayout(myMainLayout);

  // default vertical size policy is preferred
  aMainLayout->addWidget(myGroupBox);
  connect(myGroupBox, SIGNAL(clicked(bool)), this, SLOT(onPageClicked()));
}

ModuleBase_WidgetCheckGroupBox::~ModuleBase_WidgetCheckGroupBox()
{
}

void ModuleBase_WidgetCheckGroupBox::setTitle(const QString& theTitle)
{
  myGroupBox->setTitle(theTitle);
}

QWidget* ModuleBase_WidgetCheckGroupBox::pageWidget()
{
  return myGroupBox;
}

QList<QWidget*> ModuleBase_WidgetCheckGroupBox::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myGroupBox);

  return aControls;
}

void ModuleBase_WidgetCheckGroupBox::onPageClicked()
{
  storeValue();
}

void ModuleBase_WidgetCheckGroupBox::addPageStretch()
{
}

void ModuleBase_WidgetCheckGroupBox::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  const int kCol = 0;
  const int kRow = myMainLayout->count();
  // it seems, that the align on left is not necessary here, but leads to widgets, which are
  // not extended on full width of the parent page. The case is grouped widgets in
  // the sketch translation operation
  myMainLayout->addWidget(theWidget, kRow, kCol, Qt::AlignTop);// | Qt::AlignLeft);
  myMainLayout->setRowStretch(kRow, 0);

}

void ModuleBase_WidgetCheckGroupBox::placeWidget(QWidget* theWidget)
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

QLayout* ModuleBase_WidgetCheckGroupBox::pageLayout()
{
  return myMainLayout;
}

bool ModuleBase_WidgetCheckGroupBox::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aBool = aData->boolean(attributeID());
  aBool->setValue(myGroupBox->isChecked());
  updateObject(myFeature);

  return true;
}

bool ModuleBase_WidgetCheckGroupBox::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aRef = aData->boolean(attributeID());

  bool isBlocked = myGroupBox->blockSignals(true);
  myGroupBox->setChecked(aRef->value());
  myGroupBox->blockSignals(isBlocked);

  return true;
}

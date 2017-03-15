// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_WidgetOptionalBox.cpp
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetOptionalBox.h>
#include <ModelAPI_AttributeString.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include <QList>

ModuleBase_WidgetOptionalBox::ModuleBase_WidgetOptionalBox(QWidget* theParent,
                                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData),
  ModuleBase_PageBase(),
  myOptionType(CheckBox),
  myCheckBoxFrame(0),
  myCheckBox(0),
  myCheckBoxLayout(0),
  myCheckBoxWidget(0),
  myGroupBox(0),
  myGroupBoxLayout(0)
{
  myToolTip = theData->widgetTooltip();
  myGroupTitle = theData->getProperty(CONTAINER_PAGE_NAME);

  bool isChecked = theData->getBooleanAttribute(ATTR_DEFAULT, false);
  setDefaultValue(isChecked ? "true" : "false");

  myMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);
}

ModuleBase_WidgetOptionalBox::~ModuleBase_WidgetOptionalBox()
{
}

QWidget* ModuleBase_WidgetOptionalBox::pageWidget()
{
  return myOptionType == GroupBox ? myGroupBox : (QWidget*)myCheckBoxFrame;
}

QList<QWidget*> ModuleBase_WidgetOptionalBox::getControls() const
{
  QList<QWidget*> aControls;
  if (myOptionType == GroupBox)
    aControls.append(myGroupBox);
  else
    aControls.append(myCheckBoxFrame);

  return aControls;
}

void ModuleBase_WidgetOptionalBox::onPageClicked()
{
  storeValue();
  updateControlsVisibility();

  if (!isEditingMode())
    emit focusOutWidget(this);
}

void ModuleBase_WidgetOptionalBox::addPageStretch()
{
}

void ModuleBase_WidgetOptionalBox::placeModelWidget(ModuleBase_ModelWidget* theWidget)
{
  if(!isCheckBoxFilled() && myOptionType == CheckBox) {
    createControl(CheckBox);
    setOptionType(CheckBox);

    myCheckBoxWidget = theWidget; /// check box frame becomes filled
    myCheckBoxLayout->addWidget(theWidget);
  }
  else {
    createControl(GroupBox);
    setOptionType(GroupBox);

    ModuleBase_ModelWidget* aCheckBoxWidget = myCheckBoxWidget;
    myCheckBoxWidget = 0;
    if (aCheckBoxWidget) // move the model widget from check box frame to group box frame
      placeModelWidget(aCheckBoxWidget);

    const int kCol = 0;
    const int kRow = myGroupBoxLayout->count();
    // it seems, that the align on left is not necessary here, but leads to widgets, which are
    // not extended on full width of the parent page. The case is grouped widgets in
    // the sketch translation operation
    myGroupBoxLayout->addWidget(theWidget, kRow, kCol, Qt::AlignTop);// | Qt::AlignLeft);
    myGroupBoxLayout->setRowStretch(kRow, 0);
  }
}

void ModuleBase_WidgetOptionalBox::placeWidget(QWidget* theWidget)
{
  createControl(GroupBox);
  setOptionType(GroupBox);
  if (isCheckBoxFilled())
    placeModelWidget(myCheckBoxWidget);


  if (!theWidget) {
#ifdef _DEBUG
    std::cout << "ModuleBase_PageGroupBox::placePageWidget: can not cast page" << std::endl;
#endif
    return;
  }
  const int kCol = 0;
  const int kRow = myGroupBoxLayout->count();
  myGroupBoxLayout->addWidget(theWidget, kRow, kCol);
  myGroupBoxLayout->setRowStretch(kRow, 0);
}

QLayout* ModuleBase_WidgetOptionalBox::pageLayout()
{
  return myOptionType == GroupBox ? myGroupBoxLayout : (QLayout*)myCheckBoxLayout;
}

void ModuleBase_WidgetOptionalBox::createControl(const OptionType& theType)
{
  if (theType == GroupBox && !myGroupBox) {
    // group box: more than one model widget is inside
    myGroupBox = new QGroupBox(this);
    myGroupBox->setTitle(translate(myGroupTitle));
    myGroupBox->setVisible(false);
    myGroupBox->setCheckable(true);
    myGroupBox->setChecked(getDefaultValue() == "true");
    myGroupBox->setToolTip(translate(myToolTip));

    myGroupBoxLayout = new QGridLayout(myGroupBox);
    ModuleBase_Tools::zeroMargins(myGroupBoxLayout);
    myGroupBox->setLayout(myGroupBoxLayout);

    // default vertical size policy is preferred
    myMainLayout->addWidget(myGroupBox);
    connect(myGroupBox, SIGNAL(clicked(bool)), this, SLOT(onPageClicked()));
  }
  else if (theType == CheckBox && !myCheckBoxFrame) {
    myCheckBoxFrame = new QFrame(this);
    myMainLayout->addWidget(myCheckBoxFrame);

    myCheckBoxLayout = new QHBoxLayout(myCheckBoxFrame);
    ModuleBase_Tools::adjustMargins(myCheckBoxLayout);
    myCheckBox = new QCheckBox(myCheckBoxFrame);
    myCheckBox->setChecked(getDefaultValue() == "true");
    myCheckBoxLayout->addWidget(myCheckBox);

    connect(myCheckBox, SIGNAL(clicked(bool)), this, SLOT(onPageClicked()));
  }
}

bool ModuleBase_WidgetOptionalBox::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  aStringAttr->setValue(getCurrentValue() ? attributeID() : "");

  updateObject(myFeature);

  return true;
}

bool ModuleBase_WidgetOptionalBox::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  setCurrentValue(!aStringAttr->value().empty());

  return true;
}

void ModuleBase_WidgetOptionalBox::setOptionType(
                         const ModuleBase_WidgetOptionalBox::OptionType& theType)
{
  myOptionType = theType;

  bool isGroupBox = myOptionType == GroupBox;
  if (myCheckBoxFrame)
    myCheckBoxFrame->setVisible(!isGroupBox);
  if (myGroupBox)
    myGroupBox->setVisible(isGroupBox);
}

bool ModuleBase_WidgetOptionalBox::isCheckBoxFilled() const
{
  return myCheckBoxWidget != 0;
}

bool ModuleBase_WidgetOptionalBox::getCurrentValue() const
{
  bool isGroupBox = myOptionType == GroupBox;
  return isGroupBox ? myGroupBox->isChecked() : myCheckBox->isChecked();
}

void ModuleBase_WidgetOptionalBox::setCurrentValue(const bool& theValue)
{
  bool isGroupBox = myOptionType == GroupBox;
  if (isGroupBox) {
    bool isBlocked = myGroupBox->blockSignals(true);
    myGroupBox->setChecked(theValue);
    myGroupBox->blockSignals(isBlocked);
  }
  else {
    bool isBlocked = myCheckBox->blockSignals(true);
    myCheckBox->setChecked(theValue);
    myCheckBox->blockSignals(isBlocked);
  }
  updateControlsVisibility();
}

void ModuleBase_WidgetOptionalBox::updateControlsVisibility()
{
  if (myOptionType == GroupBox) {
    bool aChecked = myGroupBox->isChecked();
    ModuleBase_Tools::adjustMargins(myGroupBoxLayout);

    int aNbSubControls = myGroupBoxLayout->count();
    for (int i = 0; i < aNbSubControls; i++) {
      QWidget* aWidget = myGroupBoxLayout->itemAt(i)->widget();
      if (aWidget)
        aWidget->setEnabled(aChecked);
    }
  }
  else {
    bool aChecked = myCheckBox->isChecked();
    myCheckBoxWidget->setEnabled(aChecked);
  }
}

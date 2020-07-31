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

#include <ModuleBase_WidgetOptionalBox.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeBoolean.h>

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
  myGroupBoxLayout(0),
  myCheckGroup(0),
  myCheckGroupBtn(0),
  myCheckContent(0),
  myCheckGroupLayout(0),
  myEnableOnCheck(true)
{
  myToolTip = theData->widgetTooltip();
  myGroupTitle = theData->getProperty(CONTAINER_PAGE_NAME);

  myHaveFrame = theData->getBooleanAttribute("has_frame", true);
  myEnableOnCheck = theData->getBooleanAttribute("enable_on_check", true);
  myAlwaysShowTitle = theData->getBooleanAttribute("show_title", false);

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
  return myOptionType == GroupBox ? (myGroupBox? myGroupBox : myCheckGroup) :
    (QWidget*)myCheckBoxFrame;
}

QList<QWidget*> ModuleBase_WidgetOptionalBox::getControls() const
{
  QList<QWidget*> aControls;
  if (myOptionType == GroupBox) {
    if (myGroupBox)
      aControls.append(myGroupBox);
    else
      aControls.append(myCheckGroup);
  }
  else
    aControls.append(myCheckBoxFrame);

  return aControls;
}

void ModuleBase_WidgetOptionalBox::onPageClicked()
{
  storeValue();
  updateControlsVisibility();

  // The widget doesn't accept a focus
  //if (!isEditingMode())
  //  emit focusOutWidget(this);
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
    myCheckBoxLayout->addWidget(theWidget, 1);
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
  if (myGroupBoxLayout) {
    const int kCol = 0;
    const int kRow = myGroupBoxLayout->count();
    myGroupBoxLayout->addWidget(theWidget, kRow, kCol);
    myGroupBoxLayout->setRowStretch(kRow, 0);
  }
  else {
    myCheckGroupLayout->addWidget(theWidget);
  }
}

QLayout* ModuleBase_WidgetOptionalBox::pageLayout()
{
  return myOptionType == GroupBox ? myGroupBoxLayout : (QLayout*)myCheckBoxLayout;
}

void ModuleBase_WidgetOptionalBox::createControl(const OptionType& theType)
{
  if (theType == GroupBox && !myGroupBox) {
    // group box: more than one model widget is inside
    if (myHaveFrame) {
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
    else {
      myCheckGroup = new QWidget(this);
      QVBoxLayout* aLayout = new QVBoxLayout(myCheckGroup);
      ModuleBase_Tools::zeroMargins(aLayout);

      myCheckGroupBtn = new QCheckBox(translate(myGroupTitle), myCheckGroup);
      aLayout->addWidget(myCheckGroupBtn);

      myCheckContent = new QWidget(myCheckGroup);
      myCheckGroupLayout = new QVBoxLayout(myCheckContent);
      ModuleBase_Tools::zeroMargins(myCheckGroupLayout);
      aLayout->addWidget(myCheckContent);

      myMainLayout->addWidget(myCheckGroup);
      connect(myCheckGroupBtn, SIGNAL(toggled(bool)), this, SLOT(onPageClicked()));
    }
  }
  else if (theType == CheckBox && !myCheckBoxFrame) {
    myCheckBoxFrame = new QFrame(this);
    myMainLayout->addWidget(myCheckBoxFrame);

    myCheckBoxLayout = new QHBoxLayout(myCheckBoxFrame);
    ModuleBase_Tools::adjustMargins(myCheckBoxLayout);
    if (myAlwaysShowTitle)
      myCheckBox = new QCheckBox(translate(myGroupTitle), myCheckBoxFrame);
    else
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
  if (aStringAttr.get())
    aStringAttr->setValue(getCurrentValue() ? attributeID() : "");
  else {
    AttributeBooleanPtr aBoolAtr = aData->boolean(attributeID());
    aBoolAtr->setValue(getCurrentValue());
  }

  updateObject(myFeature);

  return true;
}

bool ModuleBase_WidgetOptionalBox::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  if (aStringAttr.get())
    setCurrentValue(!aStringAttr->value().empty());
  else {
    AttributeBooleanPtr aBoolAtr = aData->boolean(attributeID());
    setCurrentValue(aBoolAtr->value());
  }
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
  else if (myCheckContent)
    myCheckGroup->setVisible(isGroupBox);
}

bool ModuleBase_WidgetOptionalBox::isCheckBoxFilled() const
{
  return myCheckBoxWidget != 0;
}

bool ModuleBase_WidgetOptionalBox::getCurrentValue() const
{
  bool isGroupBox = myOptionType == GroupBox;
  return isGroupBox ? (myGroupBox? myGroupBox->isChecked() : myCheckGroupBtn->isChecked()) :
    myCheckBox->isChecked();
}

void ModuleBase_WidgetOptionalBox::setCurrentValue(const bool& theValue)
{
  bool isGroupBox = myOptionType == GroupBox;
  if (isGroupBox) {
    if (myGroupBox) {
      bool isBlocked = myGroupBox->blockSignals(true);
      myGroupBox->setChecked(theValue);
      myGroupBox->blockSignals(isBlocked);
    }
    else {
      bool isBlocked = myCheckGroupBtn->blockSignals(true);
      myCheckGroupBtn->setChecked(theValue);
      myCheckGroupBtn->blockSignals(isBlocked);
    }
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
    bool aChecked = toEnableWidgets();
    ModuleBase_Tools::adjustMargins(myGroupBoxLayout);

    QLayout* aLayout = myGroupBoxLayout ? myGroupBoxLayout : (QLayout*)myCheckGroupLayout;

    int aNbSubControls = aLayout->count();
    for (int i = 0; i < aNbSubControls; i++) {
      QWidget* aWidget = aLayout->itemAt(i)->widget();
      if (aWidget)
        aWidget->setEnabled(aChecked);
    }
  }
  else {
    myCheckBoxWidget->setEnabled(toEnableWidgets());
  }
}

bool ModuleBase_WidgetOptionalBox::toEnableWidgets() const
{
  bool aChecked;
  if (myOptionType == GroupBox)
    aChecked = myGroupBox ? myGroupBox->isChecked() : myCheckGroupBtn->isChecked();
  else
    aChecked = myCheckBox->isChecked();

  if (myEnableOnCheck)
    return aChecked;
  else
    return !aChecked;
}

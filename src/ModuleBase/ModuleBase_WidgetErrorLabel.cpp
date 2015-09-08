// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetErrorLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_WidgetErrorLabel.h"

#include <Config_WidgetAPI.h>
#include <ModuleBase_Tools.h>
#include <ModelAPI_AttributeString.h>

#include <QLabel>

ModuleBase_WidgetErrorLabel::ModuleBase_WidgetErrorLabel(QWidget* theParent,
                                               const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : ModuleBase_WidgetLabel(theParent, theData, theParentId)
{
  myDefaultStyle = myLabel->styleSheet();
}

ModuleBase_WidgetErrorLabel::~ModuleBase_WidgetErrorLabel()
{
}

bool ModuleBase_WidgetErrorLabel::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStrAttr = aData->string(attributeID());
  std::string aMsg;
  if (aStrAttr.get()) {
    aMsg = aStrAttr->value();
  }
  if (aMsg.empty()) {
    myLabel->setText("");
    myLabel->setStyleSheet(myDefaultStyle);
  } else {
    myLabel->setText(aMsg.c_str());
    myLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
  }
  return true;
}

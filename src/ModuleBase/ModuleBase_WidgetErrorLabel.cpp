// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetErrorLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_WidgetErrorLabel.h"

#include <Config_WidgetAPI.h>
#include <ModuleBase_Tools.h>
#include <ModelAPI_AttributeString.h>

#include <QLabel>
#include <QEvent>

ModuleBase_WidgetErrorLabel::ModuleBase_WidgetErrorLabel(QWidget* theParent,
                                               const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : ModuleBase_WidgetLabel(theParent, theData, theParentId)
{
  myDefaultStyle = myLabel->styleSheet();
  myLabel->setContentsMargins(0,0,0,4);
  myLabel->setWordWrap(true);
  myLabel->installEventFilter(this);
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
    //myLabel->setStyleSheet("QLabel { color : red; font : italic }");
    myLabel->setStyleSheet("QLabel { color : red; font : bold }");
    //myLabel->setStyleSheet("QLabel { border: 1px solid red; }");
  }
  return true;
}


bool ModuleBase_WidgetErrorLabel::focusTo() 
{ 
  restoreValue();
  return false; 
}

bool ModuleBase_WidgetErrorLabel::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myLabel) {
    if (theEvent->type() == QEvent::Show)
      restoreValue();
  }
  return ModuleBase_WidgetLabel::eventFilter(theObj, theEvent);
}

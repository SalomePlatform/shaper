// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

#include "ModuleBase_WidgetLabel.h"

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>

#include <QLabel>
#include <QVBoxLayout>


ModuleBase_WidgetLabel::ModuleBase_WidgetLabel(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aText = translate(theData->getProperty("title"));
  QString aLabelIcon = QString::fromStdString(theData->getProperty("icon"));
  myLabel = new QLabel(aText, theParent);
  if (!aLabelIcon.isEmpty()) {
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
    myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  } else {
    myLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  }
  myLabel->setWordWrap(true);
  myLabel->setIndent(5);
  myLabel->setContentsMargins(0,0,0,4);

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myLabel);
  setLayout(aLayout);

  std::string aStyleSheet = theData->getProperty(ATTR_STYLE_SHEET).c_str();
  if (!aStyleSheet.empty())
    myLabel->setStyleSheet(QString("QLabel {%1}").arg(aStyleSheet.c_str()));
}

ModuleBase_WidgetLabel::~ModuleBase_WidgetLabel()
{
}

QList<QWidget*> ModuleBase_WidgetLabel::getControls() const
{
  return QList<QWidget*>();
}

bool ModuleBase_WidgetLabel::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStrAttr = aData->string(attributeID());
  if (aStrAttr.get()) {
    std::string aMsg;
    if (aStrAttr.get()) {
      aMsg = aStrAttr->value();
    }
    QString aText = ModuleBase_Tools::translate(myFeature->getKind(), aMsg);
    myLabel->setText(aText);
  }
  return true;
}

bool ModuleBase_WidgetLabel::focusTo()
{
  restoreValue();
  return false;
}

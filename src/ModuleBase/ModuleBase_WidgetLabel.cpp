// File:        ModuleBase_WidgetLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_WidgetLabel.h"

#include <Config_WidgetAPI.h>

#include <QLabel>


ModuleBase_WidgetLabel::ModuleBase_WidgetLabel(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  QString aText = QString::fromStdString(theData->getProperty("title"));
  myLabel = new QLabel(aText, theParent);
  myLabel->setWordWrap(true);
  myLabel->setIndent(5);
}

ModuleBase_WidgetLabel::~ModuleBase_WidgetLabel()
{
}

QList<QWidget*> ModuleBase_WidgetLabel::getControls() const
{
  return QList<QWidget*>();
}

QWidget* ModuleBase_WidgetLabel::getControl() const
{
  return myLabel;
}

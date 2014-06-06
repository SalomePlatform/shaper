// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ModelWidget.h"

#include "Config_WidgetAPI.h"

#include <QWidget>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QObject* theParent, const Config_WidgetAPI* theData)
 : QObject(theParent)
{
  myAttributeID = theData ? theData->widgetId() : "";
}

bool ModuleBase_ModelWidget::canFocusTo(const std::string& theAttributeName) const
{
  return theAttributeName == attributeID();
}

void ModuleBase_ModelWidget::focusTo()
{
  QList<QWidget*> aControls = getControls();
  QList<QWidget*>::const_iterator anIt = aControls.begin(), aLast = aControls.end();
  for (; anIt != aLast; anIt++) {
    QWidget* aWidget = *anIt;
    if (aWidget && aWidget->focusPolicy() != Qt::NoFocus) {
      aWidget->setFocus();
      break;
    }
  }
}

std::string ModuleBase_ModelWidget::attributeID() const
{
  return myAttributeID;
}

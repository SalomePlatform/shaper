// File:        ModuleBase_WidgetLabel.h
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_WidgetLabel_H
#define ModuleBase_WidgetLabel_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QLabel;

class MODULEBASE_EXPORT ModuleBase_WidgetLabel : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  ModuleBase_WidgetLabel(QWidget* theParent, const Config_WidgetAPI* theData,
                            const std::string& theParentId);

  virtual ~ModuleBase_WidgetLabel();

  virtual bool storeValue() const
  {
    return true;
  }

  virtual bool restoreValue()
  {
    return true;
  }

  virtual QList<QWidget*> getControls() const;

  QWidget* getControl() const;

private:
  QLabel* myLabel;
};

#endif
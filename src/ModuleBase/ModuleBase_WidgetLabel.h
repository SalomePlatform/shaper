// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabel.h
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_WidgetLabel_H
#define ModuleBase_WidgetLabel_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QLabel;

/**
* Implementation of model widget for a label control
*/
class MODULEBASE_EXPORT ModuleBase_WidgetLabel : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetLabel(QWidget* theParent, const Config_WidgetAPI* theData,
                            const std::string& theParentId);

  virtual ~ModuleBase_WidgetLabel();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canSetValue() const { return false; };

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

  /// This control doesn't accept focus
  virtual bool focusTo() { return false; }

private:
  /// A label control
  QLabel* myLabel;
};

#endif
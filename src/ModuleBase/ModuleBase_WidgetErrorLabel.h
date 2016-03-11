// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetErrorLabel.h
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_WidgetErrorLabel_H
#define ModuleBase_WidgetErrorLabel_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetLabel.h"

class QLabel;

/**
* \ingroup GUI
* Implementation of model widget for a label with error message
*/
class MODULEBASE_EXPORT ModuleBase_WidgetErrorLabel : public ModuleBase_WidgetLabel
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetErrorLabel(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetErrorLabel();

  virtual bool restoreValueCustom();

  virtual bool focusTo();

protected:
  bool eventFilter(QObject* theObj, QEvent* theEvent);

private:
  QString myDefaultStyle;
};

#endif

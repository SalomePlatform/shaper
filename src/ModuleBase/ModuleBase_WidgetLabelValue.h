// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabelValue.h
// Created:     30 Nov 2016
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetLabelValue_H
#define ModuleBase_WidgetLabelValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_LabelValue;

/**
* \ingroup GUI
* Implementation of model widget for a label control
*/
class MODULEBASE_EXPORT ModuleBase_WidgetLabelValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetLabelValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetLabelValue();

  virtual bool restoreValueCustom();

  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

private:
  ModuleBase_LabelValue* myLabel;  ///< A label control
};

#endif

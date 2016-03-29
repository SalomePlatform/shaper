// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetBoolValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetBoolValue_H
#define ModuleBase_WidgetBoolValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QCheckBox;

/**
* \ingroup GUI
* Implementation of widget for boolean input (check box)
*/ 
class MODULEBASE_EXPORT ModuleBase_WidgetBoolValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetBoolValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetBoolValue();

  virtual bool canSetValue() const { return false; };

  virtual QList<QWidget*> getControls() const;

  virtual void setHighlighted(bool isHighlighted);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

 private:
   /// The check box
  QCheckBox* myCheckBox;
};

#endif

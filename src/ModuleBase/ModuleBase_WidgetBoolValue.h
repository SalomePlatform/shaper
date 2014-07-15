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

class MODULEBASE_EXPORT ModuleBase_WidgetBoolValue: public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetBoolValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetBoolValue();

  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue(ObjectPtr theObject) const;

  virtual bool restoreValue(ObjectPtr theObject);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

private:
  QCheckBox* myCheckBox;
};

#endif
// File:        ModuleBase_WidgetBoolValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetBoolValue_H
#define ModuleBase_WidgetBoolValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;

class MODULEBASE_EXPORT ModuleBase_WidgetBoolValue: public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_WidgetBoolValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetBoolValue();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

private:
  std::string myAttributeID;
  
  QCheckBox* myCheckBox;
};

#endif
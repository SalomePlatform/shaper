// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Widgets_H
#define ModuleBase_Widgets_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;

class MODULEBASE_EXPORT ModuleBase_DoubleValueWidget: public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_DoubleValueWidget(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_DoubleValueWidget();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const { return myContainer; }

private:
  std::string myAttributeID;
  
  QWidget*     myContainer;
  QLabel*      myLabel;
  QDoubleSpinBox* mySpinBox;
};


//////////////////////////////////////////////////////////////////////////////////

class MODULEBASE_EXPORT ModuleBase_BoolValueWidget: public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_BoolValueWidget(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_BoolValueWidget();

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
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SamplePanelPlugin_ModelWidget_H_
#define SamplePanelPlugin_ModelWidget_H_

#include <ModuleBase_ModelWidget.h>

#include <ModelAPI_Feature.h>

class Config_WidgetAPI;

class QWidget;
class QComboBox;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class SamplePanelPlugin_ModelWidget : public ModuleBase_ModelWidget
{
 Q_OBJECT
public:
  /// Constructs a model widget
  SamplePanelPlugin_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructs the model widget
  virtual ~SamplePanelPlugin_ModelWidget() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

private:
  QWidget* myMainWidget; // parent control for all widget controls
  QComboBox* myComboBox; // control for value attribute of the current feature
  int myDefaultValue; /// the default combo box value
};

#endif /* SamplePanelPlugin_ModelWidget_H_ */

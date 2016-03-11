// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetIntValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetIntValue_H
#define ModuleBase_WidgetIntValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_ParamIntSpinBox;
class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;

/**
* \ingroup GUI
* A class of property panel widget for integer value input
* It can be defined with "integervalue" keyword. For example:
* \code
* <integervalue id="x" label="X:" icon=":pictures/x_point.png" tooltip="X coordinate"/>
* \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetIntValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetIntValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetIntValue();

  /// Select the internal content if it can be selected. It is empty in the default realization
  virtual void selectContent();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns true if the event is processed.
  virtual bool processEnter();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  //! Read value of corresponded attribute from data model to the input control
  // \return True in success
  virtual bool restoreValueCustom();

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom();

protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input value control
  ModuleBase_ParamIntSpinBox* mySpinBox;
};

#endif

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetDoubleValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetDoubleValue_H
#define ModuleBase_WidgetDoubleValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_ParamSpinBox;
class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;

/**
* \ingroup GUI
* A class of property panel widget for double value input
* It can be defined with "doublevalue" keyword. For example:
* \code
* <doublevalue id="x" label="X:" icon=":pictures/x_point.png" tooltip="X coordinate"/>
* \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetDoubleValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetDoubleValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetDoubleValue();

  /// Select the internal content if it can be selected. It is empty in the default realization
  virtual void selectContent();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns true if the event is processed.
  virtual bool processEnter();

 public slots:
 // Delayed value chnged: when user starts typing something,
 // it gives him a 0,5 second to finish typing, when sends valueChnaged() signal
//  void onValueChanged();

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
  ModuleBase_ParamSpinBox* mySpinBox;
};

#endif

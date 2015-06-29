// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetIntValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetIntValue_H
#define ModuleBase_WidgetIntValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;
class QSpinBox;

/**
* \ingroup GUI
* A class of property panel widget for double value input
* It can be defined with "doublevalue" keyword. For example:
* \code
* <doublevalue id="x" label="X:" icon=":pictures/x_point.png" tooltip="X coordinate"/>
* \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetIntValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent structure (widget, operation, group)
  ModuleBase_WidgetIntValue(QWidget* theParent, const Config_WidgetAPI* theData,
                               const std::string& theParentId);

  virtual ~ModuleBase_WidgetIntValue();

  /// Fills the widget with default values
  virtual bool reset();

  //! Read value of corresponded attribute from data model to the input control
  // \return True in success
  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;


protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input value control
  QSpinBox* mySpinBox;
};

#endif

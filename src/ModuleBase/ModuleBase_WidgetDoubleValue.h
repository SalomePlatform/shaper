// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetDoubleValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetDoubleValue_H
#define ModuleBase_WidgetDoubleValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_DoubleSpinBox;
class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;

/**
* A class of property panel widget for double value input
* It can be defined with "doublevalue" keyword. For example:
* "<doublevalue id="x" label="X:" icon=":pictures/x_point.png" tooltip="X coordinate"/>"
*/
class MODULEBASE_EXPORT ModuleBase_WidgetDoubleValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent structure (widget, operation, group)
  ModuleBase_WidgetDoubleValue(QWidget* theParent, const Config_WidgetAPI* theData,
                               const std::string& theParentId);

  virtual ~ModuleBase_WidgetDoubleValue();

  //! Saves the internal parameters to the given feature
  // \return True in success
  virtual bool storeValue() const;

  //! Read value of corresponded attribute from data model to the input control
  // \return True in success
  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const
  {
    return myContainer;
  }

 public slots:
 // Delayed value chnged: when user starts typing something,
 // it gives him a 0,5 second to finish typing, when sends valueChnaged() signal
//  void onValueChanged();

 protected:
   /// Container for thw widget controls
  QWidget* myContainer;

  /// Label of the widget
  QLabel* myLabel;

  /// Input value control
  ModuleBase_DoubleSpinBox* mySpinBox;
};

#endif

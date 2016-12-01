// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_LabelValue.h
// Created:     30 Nov 2016
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_LabelValue_H
#define ModuleBase_LabelValue_H

#include "ModuleBase.h"

#include <QWidget>

class QLabel;

/**
* \ingroup GUI
* Implementation of model widget for a label control
*/
class MODULEBASE_EXPORT ModuleBase_LabelValue : public QWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theText a text value
  /// \param theToolTip a tool tip value
  /// \param theIcon a icon value
  ModuleBase_LabelValue(QWidget* theParent, const QString& theText,
                        const QString& theToolTip = "",
                        const QString& theIcon = "");

  virtual ~ModuleBase_LabelValue();

  /// Fills the label value with the given value
  /// \param theValue a value
  void setValue(const double theValue);

  /// Returns double value
  /// \return the value
  double value() const { return myValue; }

protected:
  QLabel* myLabel;  ///< A label information control
  QLabel* myLabelValue; ///< A label value control

  double myValue; ///< A cashed value to avoid a string conversion
};

#endif

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_DeflectionDialog.hxx
// Created:     27 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_DeflectionDialog_H
#define XGUI_DeflectionDialog_H

#include "XGUI.h"

#include <QDialog>

class QButtonGroup;
class QDoubleSpinBox;

/**
* \ingroup GUI
* A class of dialog to chose a deflection value. The deflection value is in range [0, 1]
*/ 
class XGUI_DeflectionDialog : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget for the dialog
  XGUI_EXPORT XGUI_DeflectionDialog(QWidget* theParent);

  XGUI_EXPORT virtual ~XGUI_DeflectionDialog() {};

  /// Initializes the dialog with the given value.
  /// \param theValue the deflection value
  void setDeflection(const double& theDeflection);

  /// Returns the current deflection value.
  /// \return a double value
  double getDeflection() const;

private:
  QButtonGroup* myButtonGroup; /// a group, contained random and certain color radio button choice
  QDoubleSpinBox* myDeflection; /// a deflection value
};

#endif

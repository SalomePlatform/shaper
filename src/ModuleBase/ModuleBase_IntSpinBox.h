// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_IntSpinBox.h
// Author:    Natalia ERMOLAEVA
//
#ifndef MODULEBASE_INT_SPINBOX_H_
#define MODULEBASE_INT_SPINBOX_H_

#include "ModuleBase.h"

#include <QSpinBox>

class QWidget;
class QKeyEvent;

/**
  * \ingroup GUI
  * Enhanced version of the Qt's int spin box.
  * It allows to store modified state
*/
class MODULEBASE_EXPORT ModuleBase_IntSpinBox : public QSpinBox
{
Q_OBJECT

public:
  explicit ModuleBase_IntSpinBox(QWidget* theParent = 0);
  virtual ~ModuleBase_IntSpinBox() {};

  /// Returns true if the current value is modified by has not been applyed yet
  virtual bool isModified() const;

  /// Clears modified state
  void clearModified();

protected slots:
  /// Called on value changed
  void onValueChanged(const QString& theValue);

 private:
  /// Boolean value whether the spin box content is modified
  bool myIsModified;
};

#endif

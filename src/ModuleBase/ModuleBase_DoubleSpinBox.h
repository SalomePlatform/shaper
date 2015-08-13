// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_DoubleSpinBox.h
// Author:    Sergey BELASH
//
#ifndef MODULEBASE_DOUBLESPINBOX_H_
#define MODULEBASE_DOUBLESPINBOX_H_

#include "ModuleBase.h"

#include <QDoubleSpinBox>
#include <QValidator>


/**
* \ingroup GUI
* An extension and customozation of QDoubleSpinBox class
*/
class MODULEBASE_EXPORT ModuleBase_DoubleSpinBox : public QDoubleSpinBox
{
Q_OBJECT

 public:
  explicit ModuleBase_DoubleSpinBox( QWidget* theParent = 0, int thePrecision = 15 );
  virtual ~ModuleBase_DoubleSpinBox();

  /// Returns true if the control is clear
  bool isCleared() const;

  /// Set control clear
  virtual void setCleared(const bool);

  /// Returns current precision
  int getPrecision() const;

  /// Set precision
  void setPrecision(const int);

  /// Set step
  virtual void stepBy(int);

  /// Converts value from string to double
  virtual double valueFromText(const QString&) const;

  /// Convert value from double to string
  virtual QString textFromValue(double) const;

  /// Validate current value
  virtual QValidator::State validate(QString&, int&) const;

 protected slots:
   /// Called on text changed
  virtual void onTextChanged(const QString&);

 protected:
   /// Removes extra trailing zero symbols
  QString removeTrailingZeroes(const QString&) const;

 private:
   /// Is clear flag
  bool myCleared;

  /// Precision value
  int myPrecision;
};

#endif

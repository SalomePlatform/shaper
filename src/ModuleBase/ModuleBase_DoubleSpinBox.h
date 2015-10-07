// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_DoubleSpinBox.h
// Author:    Sergey BELASH
//
#ifndef MODULEBASE_DOUBLESPINBOX_H_
#define MODULEBASE_DOUBLESPINBOX_H_

#include "ModuleBase.h"

#include <QDoubleSpinBox>
#include <QValidator>

class QKeyEvent;

/**
* \ingroup GUI
* An extension and customization of QDoubleSpinBox class
*/
class MODULEBASE_EXPORT ModuleBase_DoubleSpinBox : public QDoubleSpinBox
{
Q_OBJECT

 public:
  explicit ModuleBase_DoubleSpinBox( QWidget* theParent = 0, int thePrecision = -12 );
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

  virtual bool isEventProcessed(QKeyEvent* theEvent);

 protected slots:
   /// Called on text changed
  virtual void onTextChanged(const QString&);
  void onValueChanged(const QString& theValue);
  void onEditingFinished();

 protected:
   /// Removes extra trailing zero symbols
  QString removeTrailingZeroes(const QString&) const;
  virtual void keyPressEvent(QKeyEvent* theEvent);

 private:
   /// Is clear flag
  bool myCleared;

  /// Precision value
  int myPrecision;
  /// Boolean value whether the spin box content is modified
  bool myIsModified;

  QKeyEvent* myProcessedEvent;
};

#endif

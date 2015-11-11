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

  /// Returns true if the current value is modified by has not been applyed yet
  //virtual bool isModified() const;

  /// Clears modified state
  //void clearModified();

  /// Change enable/disable internal state to emit key press event
  /// \param theEnable if true, the signal is emitted
  /// \return the previous value
  bool enableKeyPressEvent(const bool& theEnable);

 protected slots:
   /// Called on text changed
  virtual void onTextChanged(const QString&);
  /// Called on value changed
  //void onValueChanged(const QString& theValue);

 protected:
   /// Removes extra trailing zero symbols
  QString removeTrailingZeroes(const QString&) const;
  /// Called on key press event
  virtual void keyPressEvent(QKeyEvent* theEvent);

 private:
  // boolen flag whether the key event is emitted. The default value is false
  bool myIsEmitKeyPressEvent;

   /// Is clear flag
  bool myCleared;

  /// Precision value
  int myPrecision;
  /// Boolean value whether the spin box content is modified
  //bool myIsModified;
};

#endif

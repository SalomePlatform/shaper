// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef MODULEBASE_DOUBLESPINBOX_H_
#define MODULEBASE_DOUBLESPINBOX_H_

#include "ModuleBase.h"

#include <QDoubleSpinBox>
#include <QValidator>

#include <QColor>

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

  /// Imitation of disable control value. If theEnable is false, the control becomes
  /// read only and base color is disabled.
  void setValueEnabled(const bool& theEnable);

 protected slots:
   /// Called on text changed
  virtual void onTextChanged(const QString&);

 protected:
   /// Removes extra trailing zero symbols
  QString removeTrailingZeroes(const QString&) const;

 private:
  // boolen flag whether the key event is emitted. The default value is false
  bool myIsEmitKeyPressEvent;

   /// Is clear flag
  bool myCleared;

  /// Precision value
  int myPrecision;

  /// Cashed color of active base palette
  QColor myEnabledBaseColor;
};

#endif

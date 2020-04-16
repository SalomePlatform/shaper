// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_ParamSpinBox_H
#define ModuleBase_ParamSpinBox_H

#include "ModuleBase.h"

#include <QAbstractSpinBox>
#include <QValidator>
#include <QLineEdit>

class QStringListModel;
class QCompleter;

/**
* \ingroup GUI
* An extension of a double spin box which let to use parameters and expressions for value definition
*/
class MODULEBASE_EXPORT ModuleBase_ParamSpinBox : public QAbstractSpinBox
{
  Q_OBJECT

  enum State { Invalid = 0, NoVariable, Incompatible, Acceptable };

public:
  /*!
   \brief Constructor.

   Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
   a step value of 1.0 and a precision of 2 decimal places.
   The value is initially set to 0.00.

   \param theParent a parent object
   \param thePrecision a precision of values display
   */
  ModuleBase_ParamSpinBox( QWidget* theParent = 0, int thePrecision = 12 );

  /// Set list of completion strings
  void setCompletionList(QStringList&);

  virtual ~ModuleBase_ParamSpinBox();

  virtual void stepBy(int);

//  virtual double valueFromText(const QString&) const;
//  virtual QString textFromValue (double value) const;

  virtual QValidator::State validate(QString&, int&) const;

  virtual void setValue(double);

  double value() const;

  virtual void setText(const QString&);

  QString text() const { return lineEdit()->text(); }

  /// Set a flag about accepted variable
  void setAcceptVariables(const bool);

  /// Returns accepted variables flag
  bool isAcceptVariables() const;

  /// Returns True if the input value contains variable
  bool hasVariable() const;

  double minimum() const { return myMinimum; }
  double maximum() const { return myMaximum; }

  void setMinimum(double theMin) { myMinimum = theMin; myValidator->setBottom(theMin); }
  void setMaximum(double theMax) { myMaximum = theMax; myValidator->setTop(theMax); }

  int decimals() const { return myValidator->decimals(); }
  void setDecimals(int thePrecision) { myValidator->setDecimals(thePrecision); }

  double singleStep() const { return mySingleStep; }
  void setSingleStep(double theStep) { mySingleStep = theStep; }

  void setValueEnabled(bool theEnable);

protected:
  /*!
  \brief This function is called when the spinbox receives key release event.
  */
  virtual void keyReleaseEvent(QKeyEvent *event);

  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

  /// The virtual function is reimplemented in order to avoid extra increasing of value by StepBy
  /// method
  virtual void timerEvent(QTimerEvent *event) {}

  virtual StepEnabled stepEnabled() const { return StepUpEnabled | StepDownEnabled; }

   /// Returns True if the given text contains variable
   /// \param theText a text string
  bool hasVariable(const QString& theText) const;

//  /// Returns state of the control
//  State isValid(const QString&, double&) const;
//
//  /// Returns True if the given value is within min and max of the control
//  bool checkRange(const double) const;
//
//  /// Finds a variable by its name. Returns true in success
//  /// \param theName a name of variable
//  /// \param outValue an output value of the variable
//  bool findVariable(const QString& theName, double& outValue) const;

signals:
  void textChanged(const QString&);

// protected:
//  virtual void showEvent(QShowEvent*);
//
// protected slots:
//   /// A slot called on text change
//  void onTextChanged(const QString&);
//
// private:
//  void connectSignalsAndSlots();

private slots:
  void insertCompletion(const QString&);

  void onTextChanged(const QString&);

private:
  QString getPrefix(int& theStart, int& theEnd) const;
  void showCompletion(bool checkPrefix);

  bool myIsEquation;
  bool myAcceptVariables;

  QStringListModel* myCompleterModel;
  QCompleter* myCompleter;

  double myMinimum;
  double myMaximum;

  int myCompletePos;

  double mySingleStep;

  /// Cashed color of active base palette
  QColor myEnabledBaseColor;

  QDoubleValidator* myValidator;
};

#endif

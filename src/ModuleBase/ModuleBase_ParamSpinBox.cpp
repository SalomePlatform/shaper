// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_ParamSpinBox.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Tools.h>

#include <QKeyEvent>
#include <QLocale>
#include <QRegExp>
#include <QToolTip>
#include <QApplication>

#include <QStringListModel>
#include <QCompleter>
#include <QAbstractItemView>
#include <QShortcut>

#include <string>
#include <iostream>

ModuleBase_ParamSpinBox::ModuleBase_ParamSpinBox(QWidget* theParent, int thePrecision)
  : QAbstractSpinBox(theParent),
  myPrecision(thePrecision),
  myIsEquation(false),
  myAcceptVariables(true),
  myDecimals(3),
  mySingleStep(1),
  myMinimum(DBL_MIN),
  myMaximum(DBL_MAX)
{
  myCompleter = new QCompleter(this);
  myCompleter->setWidget(lineEdit());
  myCompleter->setCompletionMode(QCompleter::PopupCompletion);

  myCompleterModel = new QStringListModel(this);
  myCompleter->setModel(myCompleterModel);
  // Use sorted model to accelerate completion (QCompleter will use binary search)
  myCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
  myCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  connect(myCompleter, SIGNAL(highlighted(const QString&)),
    this, SLOT(insertCompletion(const QString&)));

  //  connectSignalsAndSlots();
  myEnabledBaseColor = palette().color(QPalette::Active, QPalette::Base);
  connect(lineEdit(), SIGNAL(textChanged(const QString&)),
    this, SLOT(onTextChanged(const QString&)));

  myValidator = new QDoubleValidator(this);
  myValidator->setLocale(locale());
  myValidator->setRange(myMinimum, myMaximum);
}

void ModuleBase_ParamSpinBox::setCompletionList(QStringList& theList)
{
  theList.sort();
  theList.removeDuplicates();
  myCompleterModel->setStringList(theList);
}

/*!
 \brief Destructor.
 */
ModuleBase_ParamSpinBox::~ModuleBase_ParamSpinBox()
{
}

/*!
 \brief Perform \a steps increment/decrement steps.

 Re-implemented to handle cases when Notebook variable
 name is specified by the user as the widget text.
 Otherwise, simply calls the base implementation.

 \param steps number of increment/decrement steps
 */
void ModuleBase_ParamSpinBox::stepBy(int steps)
{
  if (hasVariable())
    return;

  double aVal = lineEdit()->text().toDouble();
  aVal += steps * mySingleStep;
  setValue(aVal);
  QAbstractSpinBox::stepBy(steps);
}

void ModuleBase_ParamSpinBox::onTextChanged(const QString& theText)
{
  myIsEquation = hasVariable(theText);
  emit textChanged(theText);
}


///*!
// \brief Connect signals and slots.
// */
//void ModuleBase_ParamSpinBox::connectSignalsAndSlots()
//{
//  connect(this, SIGNAL(valueChanged(const QString&)),
//          this, SLOT(onTextChanged(const QString&)));
//}
//
//void ModuleBase_ParamSpinBox::onTextChanged(const QString& text)
//{
//  myTextValue = text;
//  emit textChanged(text);
//}
//
//double ModuleBase_ParamSpinBox::valueFromText(const QString& theText) const
//{
//  if (!hasVariable(theText))
//    return ModuleBase_DoubleSpinBox::valueFromText(theText);
//
//  // small hack: return hash of the string to initiate valuesChanged signal
//  return qHash(theText);
//}
//
//QString ModuleBase_ParamSpinBox::textFromValue (double theValue) const
//{
//  if ((!myTextValue.isEmpty()) && hasVariable(myTextValue)){
//    return myTextValue;
//  }
//  return ModuleBase_DoubleSpinBox::textFromValue(theValue);
//}

/*!
 \brief This function is used to determine whether input is valid.
 \param str currently entered value
 \param pos cursor position in the string
 \return validating operation result
 */
QValidator::State ModuleBase_ParamSpinBox::validate(QString& str, int& pos) const
{
  // Trying to interpret the current input text as a numeric value
  if (!hasVariable(str))
    return myValidator->validate(str, pos);

  return isAcceptVariables() ? QValidator::Acceptable : QValidator::Invalid;
}

/*!
 \brief This function is used to set a current value for this spinbox.
 \param value current value

 The new value is ignored if the spinbox has a variable.
 */
void ModuleBase_ParamSpinBox::setValue(double value)
{
  myIsEquation = false;
  double aVal = value;
  if (aVal < myMinimum)
    aVal = myMinimum;
  else if (aVal > myMaximum)
    aVal = myMaximum;
  QString aText = QString::number(aVal, 'g', myDecimals);
  lineEdit()->setText(aText);
  emit textChanged(aText);
}

double ModuleBase_ParamSpinBox::value() const
{
  //if (myIsEquation) {

  //}
  return lineEdit()->text().toDouble();
}

/*!
 \brief This function is used to set a text for this spinbox.
 \param value current value
 */
void ModuleBase_ParamSpinBox::setText(const QString& value)
{
  myIsEquation = hasVariable(value);
  if (myAcceptVariables && myIsEquation) {
    lineEdit()->setText(value);
    emit textChanged(value);
  }
}

/*!
 \brief Enables or disables variable names in the spin box.
 By default, variable names are enabled.
 \param flag If true, variable names are enabled.
 */
void ModuleBase_ParamSpinBox::setAcceptVariables(const bool flag)
{
  myAcceptVariables = flag;
  if ((!myAcceptVariables) && myIsEquation) {
    setValue(0);
  }
}

/*!
 \brief Returns true if the spin box accepts variable names.
 */
bool ModuleBase_ParamSpinBox::isAcceptVariables() const
{
  return myAcceptVariables;
}

bool ModuleBase_ParamSpinBox::hasVariable() const
{
  return myIsEquation;
}

bool ModuleBase_ParamSpinBox::hasVariable(const QString& theText) const
{
  bool isDouble = false;
  QLocale::c().toDouble(theText, &isDouble);
  return !isDouble;
}

///*!
// \brief This function is used to determine whether input is valid.
// \return validating operation result
// */
//ModuleBase_ParamSpinBox::State ModuleBase_ParamSpinBox::isValid(const QString& theText,
//                                                                double& theValue) const
//{
//  if (hasVariable() && !findVariable(theText, theValue)) {
//    bool ok = false;
//    theValue = locale().toDouble(theText, &ok);
//    if (!ok) {
//      return NoVariable;
//    }
//  }
//  if (!checkRange(theValue)) {
//    return Invalid;
//  }
//
//  return Acceptable;
//}
//
///*!
// \brief This function is used to check that string value lies within predefined range.
// \return check status
// */
//bool ModuleBase_ParamSpinBox::checkRange(const double theValue) const
//{
//  return theValue >= minimum() && theValue <= maximum();
//}
//
///*!
// \brief This function is used to determine whether input is a variable name and to get its value.
// \return status of search operation
// */
//bool ModuleBase_ParamSpinBox::findVariable(const QString& theName,
//                                           double& outValue) const
//{
//  ResultParameterPtr aParam;
//  return ModelAPI_Tools::findVariable(FeaturePtr(), theName.toStdString(), outValue, aParam);
//}

/*!
 \brief This function is called when the spinbox receives key press event.
 */
void ModuleBase_ParamSpinBox::keyReleaseEvent(QKeyEvent* e)
{
  switch (e->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    if (myCompleter->popup()->isVisible()) {
      myCompleter->popup()->hide();
      myIsEquation = true;
    }
    emit textChanged(lineEdit()->text());
    return;
  case Qt::Key_Space:
    if (e->modifiers() & Qt::ControlModifier) {
      myCompletePos = lineEdit()->cursorPosition();
      int aStart, aEnd;
      QString aPrefix = getPrefix(aStart, aEnd);
      myCompleter->setCompletionPrefix(aPrefix);
      myCompleter->complete();
    }
    break;
  default:
    QAbstractSpinBox::keyReleaseEvent(e);
  }
}

QString ModuleBase_ParamSpinBox::getPrefix(int& theStart, int& theEnd) const
{
  QString aPrefix;
  QString aText = lineEdit()->text();
  theStart = theEnd = myCompletePos;
  const int aLen = aText.length();
  if (aLen > 0) {
    if (myCompletePos > 0) {
      int aLastChar = myCompletePos - 1;
      QChar aChar = aText.at(aLastChar);
      while (aChar.isLetter() || aChar.isDigit()) {
        aPrefix.prepend(aText.at(aLastChar));
        aLastChar--;
        if (aLastChar < 0)
          break;
        aChar = aText.at(aLastChar);
      }
      theStart = aLastChar + 1;
    }
    if (myCompletePos < aLen) {
      int aLastChar = myCompletePos;
      QChar aChar = aText.at(aLastChar);
      while (aChar.isLetter() || aChar.isDigit()) {
        aPrefix.append(aText.at(aLastChar));
        aLastChar++;
        if (aLastChar >= aLen)
          break;
        aChar = aText.at(aLastChar);
      }
      theEnd = aLastChar;
    }
  }
  return aPrefix;
}


void ModuleBase_ParamSpinBox::insertCompletion(const QString& theText)
{
  QString aText = lineEdit()->text();
  int aStart, aEnd;
  QString aPrefix = getPrefix(aStart, aEnd);

  QString aResult;
  int aPrefLen = aPrefix.length();
  if (aPrefLen == 0)
    aResult = aText.insert(myCompletePos, theText);
  else {
    aResult = aText.left(aStart) + theText + aText.right(aText.length() - aEnd);
  }
  lineEdit()->setText(aResult);
  myIsEquation = true;

  qDebug("### aPos=%i", myCompletePos);
  qDebug("### text=%s", qPrintable(aText));
  qDebug("### prefix=%s", qPrintable(aPrefix));
  qDebug("### result=%s", qPrintable(aResult));
}


///*!
// \brief This function is called when the spinbox receives show event.
// */
//void ModuleBase_ParamSpinBox::showEvent(QShowEvent* theEvent)
//{
//  ModuleBase_DoubleSpinBox::showEvent(theEvent);
//  if ((!myTextValue.isEmpty()) && hasVariable(myTextValue)) {
//    setText(myTextValue);
//  }
//}

void ModuleBase_ParamSpinBox::setValueEnabled(bool theEnable)
{
  setReadOnly(!theEnable);

  QPalette aPal = palette();
  aPal.setColor(QPalette::All, QPalette::Base,
    theEnable ? myEnabledBaseColor : aPal.color(QPalette::Disabled, QPalette::Base));
  setPalette(aPal);
}

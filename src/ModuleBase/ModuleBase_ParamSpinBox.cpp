#include "ModuleBase_ParamSpinBox.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Tools.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QToolTip>
#include <QRegExp>

#include <string>
#include <iostream>

/*!
 \class ModuleBase_ParamSpinBox
 */

/*!
 \brief Constructor.

 Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
 a step value of 1.0 and a precision of 2 decimal places.
 The value is initially set to 0.00.

 \param parent parent object
 */
ModuleBase_ParamSpinBox::ModuleBase_ParamSpinBox(QWidget* theParent, int thePrecision)
    : ModuleBase_DoubleSpinBox(theParent, thePrecision),
      myAcceptVariables(true)
{
  connectSignalsAndSlots();
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

  ModuleBase_DoubleSpinBox::stepBy(steps);
}

/*!
 \brief Connect signals and slots.
 */
void ModuleBase_ParamSpinBox::connectSignalsAndSlots()
{
  connect(this, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onTextChanged(const QString&)));
}

/*!
 \brief This function is called when value is changed.
 */
void ModuleBase_ParamSpinBox::onTextChanged(const QString& text)
{
  myTextValue = text;
}

/*!
 \brief Interpret text entered by the user as a value.
 \param text text entered by the user
 \return mapped value
 \sa textFromValue()
 */
double ModuleBase_ParamSpinBox::valueFromText(const QString& theText) const
{
  if (!hasVariable(theText)) {
    return ModuleBase_DoubleSpinBox::valueFromText(theText);
  }
  // small hack: return length of the string to iniiate valuesChanged signal
  return theText.length();
}

QString ModuleBase_ParamSpinBox::textFromValue (double theValue) const
{
  if (hasVariable(myTextValue)){
    return myTextValue;
  }
  return ModuleBase_DoubleSpinBox::textFromValue(theValue);
}

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
    return ModuleBase_DoubleSpinBox::validate(str, pos);

  QValidator::State res = QValidator::Invalid;
  if (isAcceptVariables()) {
    res = QValidator::Acceptable;
  }
  return res;
}

/*!
 \brief This function is used to set a current value for this spinbox.
 \param value current value
 */
void ModuleBase_ParamSpinBox::setValue(const double value)
{
  ModuleBase_DoubleSpinBox::setValue(value);

  myTextValue = ModuleBase_DoubleSpinBox::textFromValue(value);
}

/*!
 \brief This function is used to set a text for this spinbox.
 \param value current value
 */
void ModuleBase_ParamSpinBox::setText(const QString& value)
{
  lineEdit()->setText(value);
}

/*!
 \brief Enables or disables variable names in the spin box.
 By default, variable names are enabled.
 \param flag If true, variable names are enabled.
 */
void ModuleBase_ParamSpinBox::setAcceptVariables(const bool flag)
{
  myAcceptVariables = flag;
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
  if (myTextValue.isEmpty())
    return false;
  return hasVariable(myTextValue);
}

bool ModuleBase_ParamSpinBox::hasVariable(const QString& theText) const
{
  QRegExp varNameMask("[-+]?[0-9]*[.]?[0-9]+([eE][-+]?[0-9]+)?");
  return !varNameMask.exactMatch(theText);
}

/*!
 \brief This function is used to determine whether input is valid.
 \return validating operation result
 */
ModuleBase_ParamSpinBox::State ModuleBase_ParamSpinBox::isValid(const QString& theText,
                                                                double& theValue) const
{
  if (hasVariable() && !findVariable(theText, theValue)) {
    bool ok = false;
    theValue = locale().toDouble(theText, &ok);
    if (!ok) {
      return NoVariable;
    }
  }
  if (!checkRange(theValue)) {
    return Invalid;
  }

  return Acceptable;
}

/*!
 \brief This function is used to check that string value lies within predefined range.
 \return check status
 */
bool ModuleBase_ParamSpinBox::checkRange(const double theValue) const
{
  return theValue >= minimum() && theValue <= maximum();
}

/*!
 \brief This function is used to determine whether input is a variable name and to get its value.
 \return status of search operation
 */
bool ModuleBase_ParamSpinBox::findVariable(const QString& theName,
                                           double& outValue) const
{
  ResultParameterPtr aParam;
  return ModelAPI_Tools::findVariable(theName.toStdString(), outValue, aParam);
}

/*!
 \brief This function is called when the spinbox recieves key press event.
 */
void ModuleBase_ParamSpinBox::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
    QWidget::keyPressEvent(e);
  } else {
    ModuleBase_DoubleSpinBox::keyPressEvent(e);
  }
}

/*!
 \brief This function is called when the spinbox recieves show event.
 */
void ModuleBase_ParamSpinBox::showEvent(QShowEvent* theEvent)
{
  ModuleBase_DoubleSpinBox::showEvent(theEvent);
  if (hasVariable(myTextValue)) {
    setText(myTextValue);
  }
}

#include "ModuleBase_ParamIntSpinBox.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Tools.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QLocale>
#include <QRegExp>
#include <QToolTip>
#include <QApplication>

#include <string>
#include <iostream>


ModuleBase_ParamIntSpinBox::ModuleBase_ParamIntSpinBox(QWidget* theParent)
    : ModuleBase_IntSpinBox(theParent),
      myAcceptVariables(true)
{
  connectSignalsAndSlots();
}

/*!
 \brief Destructor.
 */
ModuleBase_ParamIntSpinBox::~ModuleBase_ParamIntSpinBox()
{
}

/*!
 \brief Perform \a steps increment/decrement steps.

 Re-implemented to handle cases when Notebook variable
 name is specified by the user as the widget text.
 Otherwise, simply calls the base implementation.

 \param steps number of increment/decrement steps
 */
void ModuleBase_ParamIntSpinBox::stepBy(int steps)
{
  if ((!myTextValue.isEmpty()) && hasVariable())
    return;

  ModuleBase_IntSpinBox::stepBy(steps);
}

/*!
 \brief Connect signals and slots.
 */
void ModuleBase_ParamIntSpinBox::connectSignalsAndSlots()
{
  connect(this, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onTextChanged(const QString&)));
}

void ModuleBase_ParamIntSpinBox::onTextChanged(const QString& text)
{
  myTextValue = text;
}

int ModuleBase_ParamIntSpinBox::valueFromText(const QString& theText) const
{
  if (!hasVariable(theText))
    return ModuleBase_IntSpinBox::valueFromText(theText);

  // small hack: return hash of the string to initiate valuesChanged signal
  return qHash(theText);
}

QString ModuleBase_ParamIntSpinBox::textFromValue(int theValue) const
{
  if ((!myTextValue.isEmpty()) && hasVariable(myTextValue)){
    return myTextValue;
  }
  return ModuleBase_IntSpinBox::textFromValue(theValue);
}

/*!
 \brief This function is used to determine whether input is valid.
 \param str currently entered value
 \param pos cursor position in the string
 \return validating operation result
 */
QValidator::State ModuleBase_ParamIntSpinBox::validate(QString& str, int& pos) const
{
  // Trying to interpret the current input text as a numeric value
  if (!hasVariable(str))
    return ModuleBase_IntSpinBox::validate(str, pos);

  QValidator::State res = QValidator::Invalid;
  if (isAcceptVariables()) {
    res = QValidator::Acceptable;
  }
  return res;
}

/*!
 \brief This function is used to set a current value for this spinbox.
 \param value current value

 The new value is ignored if the spinbox has a variable.
 */
void ModuleBase_ParamIntSpinBox::setValue(int value)
{
  if (hasVariable())
    return;

  myTextValue = ModuleBase_IntSpinBox::textFromValue(value);
  ModuleBase_IntSpinBox::setValue(value);
}

/*!
 \brief This function is used to set a text for this spinbox.
 \param value current value
 */
void ModuleBase_ParamIntSpinBox::setText(const QString& value)
{
  myTextValue = value;
  lineEdit()->setText(value);
}

/*!
 \brief Enables or disables variable names in the spin box.
 By default, variable names are enabled.
 \param flag If true, variable names are enabled.
 */
void ModuleBase_ParamIntSpinBox::setAcceptVariables(const bool flag)
{
  myAcceptVariables = flag;
}

/*!
 \brief Returns true if the spin box accepts variable names.
 */
bool ModuleBase_ParamIntSpinBox::isAcceptVariables() const
{
  return myAcceptVariables;
}

bool ModuleBase_ParamIntSpinBox::hasVariable() const
{
  if (myTextValue.isEmpty())
    return false;
  return hasVariable(myTextValue);
}

bool ModuleBase_ParamIntSpinBox::hasVariable(const QString& theText) const
{
  bool ok = false;
  QLocale::c().toInt(theText, &ok);
  return !ok;
}

/*!
 \brief This function is used to determine whether input is valid.
 \return validating operation result
 */
ModuleBase_ParamIntSpinBox::State ModuleBase_ParamIntSpinBox::isValid(
    const QString& theText, double& theValue) const
{
  if (hasVariable() && !findVariable(theText, theValue)) {
    bool ok = false;
    theValue = locale().toInt(theText, &ok);
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
bool ModuleBase_ParamIntSpinBox::checkRange(const double theValue) const
{
  return theValue >= minimum() && theValue <= maximum();
}

/*!
 \brief This function is used to determine whether input is a variable name and to get its value.
 \return status of search operation
 */
bool ModuleBase_ParamIntSpinBox::findVariable(const QString& theName,
                                              double& outValue) const
{
  ResultParameterPtr aParam;
  return ModelAPI_Tools::findVariable(theName.toStdString(), outValue, aParam);
}

/*!
 \brief This function is called when the spinbox receives show event.
 */
void ModuleBase_ParamIntSpinBox::showEvent(QShowEvent* theEvent)
{
  ModuleBase_IntSpinBox::showEvent(theEvent);
  if ((!myTextValue.isEmpty()) && hasVariable(myTextValue)) {
    setText(myTextValue);
  }
}

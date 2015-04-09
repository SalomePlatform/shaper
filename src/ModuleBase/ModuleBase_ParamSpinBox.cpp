#include "ModuleBase_ParamSpinBox.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QToolTip>
#include <QRegExp>

#include <string>

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
      myAcceptVariables(true),
      myHasVariables(false),
      myDefaultValue(0.)
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
  QString str = text();
  QString pref = prefix();
  QString suff = suffix();

  if (pref.length() && str.startsWith(pref))
    str = str.right(str.length() - pref.length());
  if (suff.length() && str.endsWith(suff))
    str = str.left(str.length() - suff.length());

  QRegExp varNameMask("([a-z]|[A-Z]|_).*");
  if (varNameMask.exactMatch(str))
    return;

  ModuleBase_DoubleSpinBox::stepBy(steps);
}

/*!
 \brief Connect signals and slots.
 */
void ModuleBase_ParamSpinBox::connectSignalsAndSlots()
{
  connect(this, SIGNAL(editingFinished()),
          this, SLOT(onEditingFinished()));

  connect(this, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onTextChanged(const QString&)));

  //connect(lineEdit(), SIGNAL(textChanged(const QString&)),
  //        this,       SLOT(onTextChanged(const QString&)));

  //connect(lineEdit(), SIGNAL(textChanged(const QString&)),
  //        this,       SIGNAL(textChanged(const QString&)));
}

/*!
 \brief This function is called when editing is finished.
 */
void ModuleBase_ParamSpinBox::onEditingFinished()
{
  if (myTextValue.isNull())
    myTextValue = text();

  setText(myTextValue);
}

/*!
 \brief This function is called when value is changed.
 */
void ModuleBase_ParamSpinBox::onTextChanged(const QString& text)
{
  myTextValue = text;

  double value = 0;
  if (isValid(text, value) == Acceptable) {
    myCorrectValue = text;
    myHasVariables = true;
  } else {
    myHasVariables = false;
  }
}

/*!
 \brief Interpret text entered by the user as a value.
 \param text text entered by the user
 \return mapped value
 \sa textFromValue()
 */
double ModuleBase_ParamSpinBox::valueFromText(const QString& theText) const
{
  double aValue = 0;
  if (isValid(theText, aValue) == Acceptable)
    return aValue;

  return defaultValue();
}

/*!
 \brief This function is used by the spin box whenever it needs to display
 the given value.

 \param val spin box value
 \return text representation of the value
 \sa valueFromText()
 */
QString ModuleBase_ParamSpinBox::textFromValue(double val) const
{
  return ModuleBase_DoubleSpinBox::textFromValue(val);
}

/*!
 \brief This function is used to determine whether input is valid.
 \param str currently entered value
 \param pos cursor position in the string
 \return validating operation result
 */
QValidator::State ModuleBase_ParamSpinBox::validate(QString& str, int& pos) const
{
  QValidator::State res = QValidator::Invalid;

  // Considering the input text as a variable name
  // Applying Python identifier syntax:
  // either a string starting with a letter, or a string starting with
  // an underscore followed by at least one alphanumeric character
  if (isAcceptVariables()) {
    QRegExp varNameMask("[_a-zA-Z][a-zA-Z0-9_]*");
    if (varNameMask.exactMatch(str))
      res = QValidator::Acceptable;

    if (res == QValidator::Invalid) {
      varNameMask.setPattern("_");
      if (varNameMask.exactMatch(str))
        res = QValidator::Intermediate;
    }
  }

  // Trying to interpret the current input text as a numeric value
  if (res == QValidator::Invalid)
    res = ModuleBase_DoubleSpinBox::validate(str, pos);

  return res;
}

/*!
 \brief This function is used to set a default value for this spinbox.
 \param value default value
 */
void ModuleBase_ParamSpinBox::setDefaultValue(const double value)
{
  myDefaultValue = value;
}

/*!
 \brief This function is used to set a current value for this spinbox.
 \param value current value
 */
void ModuleBase_ParamSpinBox::setValue(const double value)
{
  ModuleBase_DoubleSpinBox::setValue(value);

  myCorrectValue = ModuleBase_DoubleSpinBox::textFromValue(value);
  myTextValue = myCorrectValue;
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

bool ModuleBase_ParamSpinBox::hasVariables() const
{
  return myHasVariables;
}

/*!
 \brief This function is used to determine whether input is valid.
 \return validating operation result
 */
ModuleBase_ParamSpinBox::State ModuleBase_ParamSpinBox::isValid(const QString& theText,
                                                                double& theValue) const
{
  if (!findVariable(theText, theValue)) {
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
 \brief This function return a default acceptable value (commonly, 0.0).
 \return default acceptable value
 */
double ModuleBase_ParamSpinBox::defaultValue() const
{
  if (minimum() > myDefaultValue || maximum() < myDefaultValue)
    return minimum();

  return myDefaultValue;
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

  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDocument = aSession->activeDocument();
  ObjectPtr aParamObj = aDocument->objectByName(ModelAPI_ResultParameter::group(),
                                                theName.toStdString());
  ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
  if(!aParam.get())
    return false;
  AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
  outValue = aValueAttribute->value();
  return true;
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
void ModuleBase_ParamSpinBox::showEvent(QShowEvent*)
{
  setText(myTextValue);
}

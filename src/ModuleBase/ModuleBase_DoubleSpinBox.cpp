// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_DoubleSpinBox.cxx
// Author:    Sergey TELKOV
//
#include "ModuleBase_DoubleSpinBox.h"

#include <QLineEdit>
#include <QDoubleValidator>
#include <QVariant>
#include <QKeyEvent>

#include <limits>

const double PSEUDO_ZERO = 1.e-20;

/*!
 \class ModuleBase_DoubleSpinBox
 \brief Enhanced version of the Qt's double spin box.

 The ModuleBase_DoubleSpinBox class represents the widget for entering the
 floating point values. In addition to the functionality provided by
 QDoubleSpinBox, this class supports "cleared" state - this is the
 state corresponding to "None" (or empty) entered value.

 To set "cleared" state use setCleared() method. To check if the spin
 box stores "cleared" state, use isCleared() method.
 For example:
 \code
 if (myDblSpinBox->isCleared()) {
 ... // process "None" state
 }
 else {
 double value = myDblSpinBox->value();
 ... // process entered value
 }
 \endcode

 Another useful feature is possibility to use scientific notation (e.g. 1.234e+18)
 for the widget text. To enable this, negative precision should be specified either
 through a constructor or using setPrecision() method.

 Note that "decimals" property of QDoubleSpinBox is almost completely substituted
 by "myPrecision" field of ModuleBase_DoubleSpinBox class. "decimals" is still used
 for proper size hint calculation and for rounding minimum and maximum bounds of
 the spin box range.
 */

/*!
 \brief Constructor.

 Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
 a step value of 1.0 and a precision of 2 decimal places.
 The value is initially set to 0.00.

 \param theParent parent object
 \param thePrecision precision of values input
 */
ModuleBase_DoubleSpinBox::ModuleBase_DoubleSpinBox(QWidget* theParent, int thePrecision)
    : QDoubleSpinBox(theParent),
      myCleared(false),
      myIsModified(false),
      myIsEmitKeyPressEvent(false)
{
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid inconsistency of double-2-string and string-2-double conversion)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator | QLocale::RejectGroupSeparator);
  setLocale(loc);

  // MPV 15/09/2014: this must be set before setDecimals; otherwise in release mode setDecimals may crash
  myPrecision = thePrecision;

  // Use precision equal to default Qt decimals
  // it's necessary to set decimals before the range setting,
  // by default Qt rounds boundaries to 2 decimals at setRange
  setDecimals(qAbs(myPrecision));

  connect(lineEdit(), SIGNAL(textChanged( const QString& )), this,
          SLOT(onTextChanged( const QString& )));

  connect(this, SIGNAL(valueChanged(const QString&)), this, SLOT(onValueChanged(const QString&)));
  //connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

/*!
 \brief Destructor.
 */
ModuleBase_DoubleSpinBox::~ModuleBase_DoubleSpinBox()
{
}

/*!
 \brief Check if spin box is in the "cleared" state.
 \return \c true if spin box is cleared
 \sa setCleared()
 */
bool ModuleBase_DoubleSpinBox::isCleared() const
{
  return myCleared;
}

/*!
 \brief Change "cleared" status of the spin box.
 \param on new "cleared" status
 \sa isCleared()
 */
void ModuleBase_DoubleSpinBox::setCleared(const bool on)
{
  if (myCleared == on)
    return;

  myCleared = on;
  setSpecialValueText(specialValueText());
}

/*!
 \brief Set precision of the spin box

 If precision value is less than 0, the 'g' format is used for value output,
 otherwise 'f' format is used.

 \param prec new precision value.
 \sa precision()
 */
void ModuleBase_DoubleSpinBox::setPrecision(const int prec)
{
  int newPrec = qAbs(prec);
  int oldPrec = qAbs(myPrecision);
  myPrecision = prec;
  if (newPrec != oldPrec)
    update();
}

/*!
 \brief Get precision value of the spin box
 \return current precision value
 \sa setPrecision()
 */
int ModuleBase_DoubleSpinBox::getPrecision() const
{
  return myPrecision;
}

/*!
 \brief Interpret text entered by the user as a value.
 \param text text entered by the user
 \return mapped value
 \sa textFromValue()
 */
double ModuleBase_DoubleSpinBox::valueFromText(const QString& text) const
{
  if (myPrecision < 0)
    return text.toDouble();

  return QDoubleSpinBox::valueFromText(text);
}

/*!
 \brief This function is used by the spin box whenever it needs to display
 the given value.

 \param val spin box value
 \return text representation of the value
 \sa valueFromText()
 */
QString ModuleBase_DoubleSpinBox::textFromValue(double val) const
{
  QString s = locale().toString(val, myPrecision >= 0 ? 'f' : 'g', qAbs(myPrecision));
  return removeTrailingZeroes(s);
}

/*!
 \brief Return source string with removed leading and trailing zeros.
 \param src source string
 \return resulting string
 */
QString ModuleBase_DoubleSpinBox::removeTrailingZeroes(const QString& src) const
{
  QString delim(locale().decimalPoint());

  int idx = src.lastIndexOf(delim);
  if (idx == -1)
    return src;

  QString iPart = src.left(idx);
  QString fPart = src.mid(idx + 1);
  QString ePart = "";
  int idx1 = fPart.lastIndexOf(QRegExp("e[+|-]?[0-9]+"));
  if (idx1 >= 0) {
    ePart = fPart.mid(idx1);
    fPart = fPart.left(idx1);
  }

  fPart.remove(QRegExp("0+$"));

  QString res = iPart;
  if (!fPart.isEmpty())
    res += delim + fPart;
  res += ePart;

  return res;
}

void ModuleBase_DoubleSpinBox::keyPressEvent(QKeyEvent *theEvent)
{
  switch (theEvent->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: {
      // do not react to the Enter key, the property panel processes it
      if (!myIsEmitKeyPressEvent)
        return;
    }
    break;
    default:
      break;
  }
  QDoubleSpinBox::keyPressEvent(theEvent);
}

bool ModuleBase_DoubleSpinBox::focusNextPrevChild(bool theIsNext)
{
  myIsModified = false;

  emit valueStored();
  emit focusNextPrev();
  return QDoubleSpinBox::focusNextPrevChild(theIsNext);
}

/*!
 \brief Perform \a steps increment/decrement steps.

 The \a steps value can be any integer number. If it is > 0,
 the value incrementing is done, otherwise value is decremented
 \a steps times.

 \param steps number of increment/decrement steps
 */
void ModuleBase_DoubleSpinBox::stepBy(int steps)
{
  myCleared = false;

  QDoubleSpinBox::stepBy(steps);
  double tmpval = value();
  if (qAbs(tmpval) < PSEUDO_ZERO)
    tmpval = 0.;
  if (tmpval < minimum())
    tmpval = minimum();
  else if (tmpval > maximum())
    tmpval = maximum();
  setValue(tmpval);
}

/*!
 \brief This function is used to determine whether input is valid.
 \param str currently entered value
 \param pos cursor position in the string
 \return validating operation result
 */
QValidator::State ModuleBase_DoubleSpinBox::validate(QString& str, int& pos) const
{
  QString pref = this->prefix();
  QString suff = this->suffix();
  uint overhead = pref.length() + suff.length();
  QValidator::State state = QValidator::Invalid;

  QDoubleValidator v(NULL);

  // If 'g' format is used (myPrecision < 0), then
  // myPrecision - 1 digits are allowed after the decimal point.
  // Otherwise, expect myPrecision digits after the decimal point.
  int decs = myPrecision < 0 ? qAbs(myPrecision) - 1 : myPrecision;

  v.setLocale(this->locale());
  v.setDecimals(decs);
  v.setBottom(minimum());
  v.setTop(maximum());
  v.setNotation(
      myPrecision >= 0 ? QDoubleValidator::StandardNotation : QDoubleValidator::ScientificNotation);

  if (overhead == 0)
    state = v.validate(str, pos);
  else {
    if ((uint)(str.length()) >= overhead && str.startsWith(pref) && str.right(suff.length()) == suff) {
      QString core = str.mid(pref.length(), str.length() - overhead);
      int corePos = pos - pref.length();
      state = v.validate(core, corePos);
      pos = corePos + pref.length();
      str.replace(pref.length(), str.length() - overhead, core);
    } else {
      state = v.validate(str, pos);
      if (state == QValidator::Invalid) {
        QString special = this->specialValueText().trimmed();
        QString candidate = str.trimmed();
        if (special.startsWith(candidate)) {
          if (candidate.length() == special.length())
            state = QValidator::Acceptable;
          else
            state = QValidator::Intermediate;
        }
      }
    }
  }

  // Treat values outside (min; max) range as Invalid
  // This check is enabled by assigning "strict_validity_check" dynamic property
  // with value "true" to the spin box instance.
  if (state == QValidator::Intermediate) {
    bool isOk;
    double val = str.toDouble(&isOk);
    if (isOk) {
      QVariant propVal = property("strict_validity_check");
      if (propVal.isValid() && propVal.canConvert(QVariant::Bool) && propVal.toBool()) {
        if (val < minimum() || val > maximum())
          state = QValidator::Invalid;
      }
    } else if (myPrecision < 0) {
      // Consider too large negative exponent as Invalid
      QChar e(locale().exponential());
      int epos = str.indexOf(e, 0, Qt::CaseInsensitive);
      if (epos != -1) {
        epos++;  // Skip exponential symbol itself
        QString exponent = str.right(str.length() - epos);
        int expValue = exponent.toInt(&isOk);
        if (isOk && expValue < std::numeric_limits<double>::min_exponent10)
          state = QValidator::Invalid;
      }
    }
  }

  return state;
}

/*!
 \brief Called when user enters the text in the spin box.
 */
void ModuleBase_DoubleSpinBox::onTextChanged(const QString& )
{
  myCleared = false;
  myIsModified = true;
}

void ModuleBase_DoubleSpinBox::onValueChanged(const QString& theValue)
{
  myIsModified = true;
}

void ModuleBase_DoubleSpinBox::onEditingFinished()
{
  //myIsModified = false;
}

bool ModuleBase_DoubleSpinBox::isModified() const
{
  return myIsModified;
}

void ModuleBase_DoubleSpinBox::clearModified()
{
  myIsModified = false;
}

bool ModuleBase_DoubleSpinBox::enableKeyPressEvent(const bool& theEnable)
{
  bool aPreviousValue = myIsEmitKeyPressEvent;
  myIsEmitKeyPressEvent = theEnable;

  return aPreviousValue;
}

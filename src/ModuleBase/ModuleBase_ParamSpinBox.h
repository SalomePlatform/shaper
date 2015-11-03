
#ifndef ModuleBase_ParamSpinBox_H
#define ModuleBase_ParamSpinBox_H

#include "ModuleBase.h"

#include <ModuleBase_DoubleSpinBox.h>

#include <QValidator>

/**
* \ingroup GUI
* An extension of a double spin box which let to use parameters and expressions for value definition
*/
class MODULEBASE_EXPORT ModuleBase_ParamSpinBox : public ModuleBase_DoubleSpinBox
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
  explicit ModuleBase_ParamSpinBox( QWidget* theParent = 0, int thePrecision = -12 );
  virtual ~ModuleBase_ParamSpinBox();

  virtual void stepBy(int);

  virtual double valueFromText(const QString&) const;
  virtual QString textFromValue (double value) const;

  virtual QValidator::State validate(QString&, int&) const;

  virtual void setValue(double);

  virtual void setText(const QString&);

  /// Set a flag about accepted variable
  void setAcceptVariables(const bool);

  /// Returns accepted variables flag
  bool isAcceptVariables() const;

  /// Returns True if the input value contains variable
  bool hasVariable() const;

 protected:
   /// Returns True if the given text contains variable
   /// \param theText a text string
  bool hasVariable(const QString& theText) const;

  /// Returns state of the control
  State isValid(const QString&, double&) const;

  /// Returns True if the given value is within min and max of the control
  bool checkRange(const double) const;

  /// Finds a variable by its name. Rreturns true in success
  /// \param theName a name of variable
  /// \param outValue an output val;ue of the variable
  bool findVariable(const QString& theName, double& outValue) const;

 protected:
  virtual void showEvent(QShowEvent*);

 protected slots:
   /// A slot called on text change
  void onTextChanged(const QString&);

 private:
  void connectSignalsAndSlots();

 private:
  QString myTextValue;

  bool myAcceptVariables;
};

#endif

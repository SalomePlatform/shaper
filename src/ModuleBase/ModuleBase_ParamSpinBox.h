
#ifndef ModuleBase_ParamSpinBox_H
#define ModuleBase_ParamSpinBox_H

#include "ModuleBase.h"

#include <ModuleBase_DoubleSpinBox.h>

#include <QValidator>

class MODULEBASE_EXPORT ModuleBase_ParamSpinBox : public ModuleBase_DoubleSpinBox
{
  Q_OBJECT

  enum State { Invalid = 0, NoVariable, Incompatible, Acceptable };

public:
  explicit ModuleBase_ParamSpinBox(QWidget* theParent = 0, int thePrecision = 6);
  virtual ~ModuleBase_ParamSpinBox();

  virtual void stepBy(int);

  virtual double valueFromText(const QString&) const;
  virtual QString textFromValue(double) const;

  virtual QValidator::State validate(QString&, int&) const;

  virtual void setDefaultValue(const double);

  virtual void setValue(double);

  virtual void setText(const QString&);

  void setAcceptVariables(const bool);
  bool isAcceptVariables() const;
  bool hasVariables() const;

signals:
  void textChanged(const QString&);

 protected:
  State isValid(const QString&, double&) const;

  double defaultValue() const;
  bool checkRange(const double) const;

  bool findVariable(const QString&, double&) const;

 protected:
  virtual void keyPressEvent(QKeyEvent*);
  virtual void showEvent(QShowEvent*);

 protected slots:
  void onEditingFinished();
  void onTextChanged(const QString&);

 private:
  void connectSignalsAndSlots();

 private:
  double myDefaultValue;

  QString myCorrectValue;
  QString myTextValue;

  bool myAcceptVariables;
  bool myHasVariables;
};

#endif

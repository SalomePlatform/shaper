
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
  virtual QString textFromValue (double value) const;

  virtual QValidator::State validate(QString&, int&) const;

  virtual void setValue(double);

  virtual void setText(const QString&);

  void setAcceptVariables(const bool);
  bool isAcceptVariables() const;
  bool hasVariable() const;

//signals:
  //void textChanged(const QString&);

 protected:
  bool hasVariable(const QString& theText) const;
  State isValid(const QString&, double&) const;

  bool checkRange(const double) const;

  bool findVariable(const QString&, double&) const;

 protected:
  //virtual void keyPressEvent(QKeyEvent*);
  virtual void showEvent(QShowEvent*);

 protected slots:
  virtual void onTextChanged(const QString&);

 private:
  void connectSignalsAndSlots();

 private:
  QString myTextValue;

  bool myAcceptVariables;
};

#endif

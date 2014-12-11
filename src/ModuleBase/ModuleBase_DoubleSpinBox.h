// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_DoubleSpinBox.h
// Author:    Sergey TELKOV
//
#ifndef MODULEBASE_DOUBLESPINBOX_H_
#define MODULEBASE_DOUBLESPINBOX_H_

#include "ModuleBase.h"

#include <QDoubleSpinBox>
#include <QValidator>

class MODULEBASE_EXPORT ModuleBase_DoubleSpinBox : public QDoubleSpinBox
{
Q_OBJECT

 public:
  ModuleBase_DoubleSpinBox(QWidget* = 0, int thePrecision = 6);
  virtual ~ModuleBase_DoubleSpinBox();

  bool isCleared() const;
  virtual void setCleared(const bool);

  int getPrecision() const;
  void setPrecision(const int);

  virtual void stepBy(int);

  virtual double valueFromText(const QString&) const;
  virtual QString textFromValue(double) const;

  virtual QValidator::State validate(QString&, int&) const;

 protected slots:
  virtual void onTextChanged(const QString&);

 protected:
  QString removeTrailingZeroes(const QString&) const;

 private:
  bool myCleared;
  int myPrecision;
};

#endif

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_ErrorDialog.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: sbh
 */
#include <XGUI_ErrorDialog.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>

XGUI_ErrorDialog::XGUI_ErrorDialog(QWidget* parent)
    : QDialog(parent)
{
  QVBoxLayout* aDlgLay = new QVBoxLayout(this);
  setWindowTitle(tr("Application errors"));
  myErrorLog = new QTextEdit(this);
  myErrorLog->setReadOnly(true);
  aDlgLay->addWidget(myErrorLog);
  QDialogButtonBox* aButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal,
                                                      this);
  aDlgLay->addWidget(aButtonBox);
  aDlgLay->setContentsMargins(2, 2, 2, 2);
  aDlgLay->setSpacing(2);
  setLayout(aDlgLay);
  resize(420, 240);

  connect(aButtonBox, SIGNAL(accepted()), this, SLOT(clear()));
  connect(aButtonBox, SIGNAL(rejected()), this, SLOT(clear()));
}

XGUI_ErrorDialog::~XGUI_ErrorDialog()
{
}

void XGUI_ErrorDialog::refresh()
{
  myErrorLog->clear();
  foreach(QString eachError, myErrors)
  {
    myErrorLog->append(eachError);
  }
}

void XGUI_ErrorDialog::clear()
{
  myErrorLog->clear();
  myErrors.clear();
  QDialog::reject();
}

void XGUI_ErrorDialog::addError(const QString& theError)
{
  myErrors.append(theError);
  refresh();
  if (!isVisible()) {
    show();
    raise();
    activateWindow();
  }
}

void XGUI_ErrorDialog::removeError(const QString& theError)
{
  myErrors.removeAll(theError);
  refresh();
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_EditLine.h
// Created:     02 June 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_EditLine.h>

#include <QLineEdit>

PartSet_EditLine::PartSet_EditLine(QWidget* theParent)
    : QObject(theParent)
{
  myEditor = new QLineEdit(theParent);
  myEditor->setWindowFlags(Qt::ToolTip);
  myEditor->setFocusPolicy(Qt::StrongFocus);

  connect(myEditor, SIGNAL(returnPressed()), this, SLOT(onStopEditing()));
}

void PartSet_EditLine::start(const QPoint& thePoint, double theValue)
{
  myEditor->move(thePoint);
  myEditor->setText(QString::number(theValue));
  myEditor->show();

  myEditor->selectAll();
  myEditor->setFocus();
}

bool PartSet_EditLine::isStarted() const
{
  return myEditor->isVisible();
}

void PartSet_EditLine::stop()
{
  myEditor->hide();
}

double PartSet_EditLine::getValue() const
{
  return myEditor->text().toDouble();
}

void PartSet_EditLine::onStopEditing()
{
  stop();
  emit stopped(getValue());
}

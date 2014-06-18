// File:        PartSet_EditLine.h
// Created:     02 June 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_EditLine.h>

#include <QLineEdit>

PartSet_EditLine::PartSet_EditLine(QWidget* theParent)
: QObject(theParent)
{
  myEditor = new QLineEdit(theParent);

  connect(myEditor, SIGNAL(returnPressed()), this, SLOT(onStopEditing()));
}

void PartSet_EditLine::start(const QPoint& thePoint, double theValue)
{
  myEditor->move(thePoint);
  myEditor->setText(QString::number(theValue));
  myEditor->show();
}

double PartSet_EditLine::getValue() const
{
  return myEditor->text().toDouble();
}

void PartSet_EditLine::onStopEditing()
{
  myEditor->hide();
  emit stopped(getValue());
}

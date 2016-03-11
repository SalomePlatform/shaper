// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetEditor.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDouble.h>

#include <GeomDataAPI_Point2D.h>

#include <QApplication>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDesktopWidget>
#include <QDialog>
#include <QLayout>

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_WidgetDoubleValue(theParent, theData),
  myXPosition(-1), myYPosition(-1)
{
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

void ModuleBase_WidgetEditor::editedValue(double& outValue, QString& outText)
{
  QDialog aDlg(QApplication::desktop(), Qt::FramelessWindowHint);
  QHBoxLayout* aLay = new QHBoxLayout(&aDlg);
  aLay->setContentsMargins(2, 2, 2, 2);

  ModuleBase_ParamSpinBox* anEditor = new ModuleBase_ParamSpinBox(&aDlg);
  anEditor->enableKeyPressEvent(true);

  anEditor->setMinimum(0);
  anEditor->setMaximum(DBL_MAX);
  if (outText.isEmpty())
    anEditor->setValue(outValue);
  else
    anEditor->setText(outText);

  aLay->addWidget(anEditor);

  ModuleBase_Tools::setFocus(anEditor, "ModuleBase_WidgetEditor::editedValue");
  anEditor->selectAll();
  QObject::connect(anEditor, SIGNAL(enterReleased()), &aDlg, SLOT(accept()));

  QPoint aPoint = QCursor::pos();
  if (myXPosition >= 0 && myYPosition >= 0)
    aPoint = QPoint(myXPosition, myYPosition);

  aDlg.move(aPoint);
  aDlg.exec();

  outText = anEditor->text();
  bool isDouble;
  double aValue = outText.toDouble(&isDouble);
  if (isDouble) {
    outValue = aValue;
    outText = ""; // return empty string, if it's can be converted to a double
  }
}

bool ModuleBase_WidgetEditor::focusTo()
{
  showPopupEditor();
  return true;
}

void ModuleBase_WidgetEditor::showPopupEditor(const bool theSendSignals)
{
  // we need to emit the focus in event manually in order to save the widget as an active
  // in the property panel before the mouse leave event happens in the viewer. The module
  // ask an active widget and change the feature visualization if the widget is not the current one.
  if (theSendSignals)
    emit focusInWidget(this);

  // nds: it seems, that the envents processing is not necessary anymore
  // White while all events will be processed
  //QApplication::processEvents();
  double aValue = mySpinBox->value();
  QString aText;
  if (mySpinBox->hasVariable())
    aText = mySpinBox->text();

  editedValue(aValue, aText);
  if (aText.isEmpty()) {
    ModuleBase_Tools::setSpinValue(mySpinBox, aValue);
  } else {
    ModuleBase_Tools::setSpinText(mySpinBox, aText);
  }
  if (theSendSignals) {
    emit valuesChanged();
    // the focus leaves the control automatically by the Enter/Esc event
    // it is processed in operation manager
    //emit focusOutWidget(this);

    if (!myIsEditing)
      emit enterClicked(this);
  }
  else
    storeValue();
}

void ModuleBase_WidgetEditor::setCursorPosition(const int theX, const int theY)
{
  myXPosition = theX;
  myYPosition = theY;
}

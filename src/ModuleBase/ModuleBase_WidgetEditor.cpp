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

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData,
                                                 const std::string& theParentId)
    : ModuleBase_WidgetDoubleValue(theParent, theData, theParentId)
{
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

void editedValue(double& outValue, QString& outText)
{
  QMenu* aPopup = new QMenu();

  QLineEdit* aEditor = new QLineEdit(QString::number(outValue), aPopup);
  QWidgetAction* aLineEditAction = new QWidgetAction(aPopup);
  aLineEditAction->setDefaultWidget(aEditor);
  aPopup->addAction(aLineEditAction);

  aEditor->setFocus();
  aEditor->selectAll();
  QString anExpression("([0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)|([_a-zA-Z][a-zA-Z0-9_]*)");
  aEditor->setValidator(new QRegExpValidator(QRegExp(anExpression), aEditor));
  QObject::connect(aEditor, SIGNAL(returnPressed()), aLineEditAction, SIGNAL(triggered()));
  QObject::connect(aLineEditAction, SIGNAL(triggered()), aPopup, SLOT(hide()));

  QAction* aResult = aPopup->exec(QCursor::pos());
  outText = aEditor->text();
  bool isDouble;
  double aValue = outText.toDouble(&isDouble);
  if (isDouble) {
    outValue = aValue;
  //  outText = ""; // return empty string, if it's can be converted to a double
  }
  aPopup->deleteLater();
}

bool ModuleBase_WidgetEditor::focusTo()
{
  // nds: it seems, that the timer is not necessary anymore

  // We can not launch here modal process for value editing because 
  // it can be called on other focusOutWidget event and will block it
  //QTimer::singleShot(1, this, SLOT(showPopupEditor()));

  showPopupEditor();

  return true;
}

void ModuleBase_WidgetEditor::showPopupEditor()
{
  // we need to emit the focus in event manually in order to save the widget as an active
  // in the property panel before the mouse leave event happens in the viewer. The module
  // ask an active widget and change the feature visualization if the widget is not the current one.
  emit focusInWidget(this);

  // nds: it seems, that the envents processing is not necessary anymore
  // White while all events will be processed
  //QApplication::processEvents();
  double aValue = mySpinBox->value();
  QString aText;
  editedValue(aValue, aText);
  if (aText.isEmpty()) {
    ModuleBase_Tools::setSpinValue(mySpinBox, aValue);
  } else {
    ModuleBase_Tools::setSpinText(mySpinBox, aText);
  }
  emit valuesChanged();
  emit focusOutWidget(this);
}

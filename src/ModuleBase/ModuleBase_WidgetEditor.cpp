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

#include <QWidget>
#include <QLineEdit>
//#include <QTimer>
#include <QDialog>
#include <QLayout>
#include <QApplication>

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData,
                                                 const std::string& theParentId)
    : ModuleBase_WidgetDoubleValue(theParent, theData, theParentId)
{
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

double editedValue(double theValue, bool& isDone)
{
  QDialog aDlg;
  aDlg.setWindowFlags(Qt::FramelessWindowHint);
  QHBoxLayout* aLay = new QHBoxLayout(&aDlg);
  ModuleBase_Tools::zeroMargins(aLay);

  QLineEdit* aEditor = new QLineEdit(QString::number(theValue), &aDlg);
  aEditor->selectAll();
  aEditor->setValidator(new QDoubleValidator(aEditor));
  QObject::connect(aEditor, SIGNAL(returnPressed()), &aDlg, SLOT(accept()));
  aLay->addWidget(aEditor);

  QPoint aPoint = QCursor::pos();
  aDlg.move(aPoint);

  isDone = aDlg.exec() == QDialog::Accepted;
  double aValue = theValue;
  if (isDone)
    aValue = aEditor->text().toDouble();
  return aValue;
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
  bool isDone;
  aValue = editedValue(aValue, isDone);

  if (isDone) {
    ModuleBase_Tools::setSpinValue(mySpinBox, aValue);
  }
  emit valuesChanged();
  emit focusOutWidget(this);
}

void ModuleBase_WidgetEditor::editFeatureValue(FeaturePtr theFeature,
                                               const std::string theAttribute)
{
  DataPtr aData = theFeature->data();
  AttributeDoublePtr aRef = aData->real(theAttribute);
  double aValue = aRef->value();

  bool isDone;
  aValue = editedValue(aValue, isDone);
  if (isDone)
    aRef->setValue(aValue);
}

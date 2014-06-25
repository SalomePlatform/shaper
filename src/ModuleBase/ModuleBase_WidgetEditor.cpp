// File:        ModuleBase_WidgetEditor.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetEditor.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <Model_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDouble.h>

#include <GeomDataAPI_Point2D.h>

#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QDialog>
#include <QLayout>
#include <QDoubleSpinBox>

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_WidgetDoubleValue(theParent, theData)
{
}

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent, const std::string& theAttribute)
: ModuleBase_WidgetDoubleValue(theParent, 0)
{
  setAttributeID(theAttribute);
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

double editedValue(double theValue, bool& isDone)
{
  QDialog aDlg;
  aDlg.setWindowFlags(Qt::FramelessWindowHint);
  QHBoxLayout* aLay = new QHBoxLayout(&aDlg);
  aLay->setContentsMargins(0,0,0,0);

  QLineEdit* aEditor = new QLineEdit(QString::number(theValue), &aDlg);
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

void ModuleBase_WidgetEditor::focusTo()
{
  double aValue = mySpinBox->value();
  bool isDone;
  aValue = editedValue(aValue, isDone);

  if (isDone) {
    bool isBlocked = mySpinBox->blockSignals(true);
    mySpinBox->setValue(aValue);
    mySpinBox->blockSignals(isBlocked);
  }
  emit valuesChanged();
  emit focusOutWidget(this);
}

void ModuleBase_WidgetEditor::editFeatureValue(FeaturePtr theFeature, const std::string theAttribute)
{
  DataPtr aData = theFeature->data();
  AttributeDoublePtr aRef = aData->real(theAttribute);
  double aValue = aRef->value();

  bool isDone;
  aValue = editedValue(aValue, isDone);
  if (isDone)
    aRef->setValue(aValue);
}

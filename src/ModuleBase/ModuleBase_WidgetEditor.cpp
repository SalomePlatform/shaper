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

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myValue(0)
{
}

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent, const std::string& theAttribute)
: ModuleBase_ModelWidget(theParent, 0), myValue(0)
{
  this->setAttributeID(theAttribute);
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

bool ModuleBase_WidgetEditor::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal->value() != myValue) {
    aReal->setValue(myValue);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

bool ModuleBase_WidgetEditor::restoreValue(FeaturePtr theFeature)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());

  myValue = aRef->value();
  return true;
}

void ModuleBase_WidgetEditor::focusTo()
{
  QPoint aPoint = QCursor::pos();

  QDialog aDlg;
  aDlg.setWindowFlags(Qt::FramelessWindowHint);
  QHBoxLayout* aLay = new QHBoxLayout(&aDlg);
  aLay->setContentsMargins(0,0,0,0);

  QLineEdit* aEditor = new QLineEdit(QString::number(myValue), &aDlg);
  connect(aEditor, SIGNAL(returnPressed()), &aDlg, SLOT(accept()));
  aLay->addWidget(aEditor);

  aDlg.move(aPoint);
  int aRes = aDlg.exec();

  if (aRes == QDialog::Accepted)
    myValue = aEditor->text().toDouble();

  emit valuesChanged();
  emit focusOutWidget(this);
}

QWidget* ModuleBase_WidgetEditor::getControl() const
{
  return 0;
}

QList<QWidget*> ModuleBase_WidgetEditor::getControls() const
{
  QList<QWidget*> aControls;
  return aControls;
}

void ModuleBase_WidgetEditor::editFeatureValue(FeaturePtr theFeature, const std::string theAttribute)
{
  ModuleBase_WidgetEditor anEditor(0, theAttribute);

  anEditor.restoreValue(theFeature);
  anEditor.focusTo();
  anEditor.storeValue(theFeature);
}

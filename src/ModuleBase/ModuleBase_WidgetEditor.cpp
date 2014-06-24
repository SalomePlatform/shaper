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

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  myEditor = new QLineEdit(0);
  myEditor->setWindowFlags(Qt::ToolTip);
  myEditor->setFocusPolicy(Qt::StrongFocus);

  connect(myEditor, SIGNAL(returnPressed()), this, SLOT(onStopEditing()));
  connect(myEditor, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
  delete myEditor;
}

bool ModuleBase_WidgetEditor::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  bool isOk;
  double aValue = myEditor->text().toDouble(&isOk);
  if (isOk && aReal->value() != aValue) {
    //ModuleBase_WidgetPoint2D* that = (ModuleBase_WidgetPoint2D*) this;
    //bool isBlocked = that->blockSignals(true);
    aReal->setValue(aValue);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
    //that->blockSignals(isBlocked);
  }
  return true;
}

bool ModuleBase_WidgetEditor::restoreValue(FeaturePtr theFeature)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());

  //bool isBlocked = this->blockSignals(true);
  myEditor->setText(QString::number(aRef->value()));
  //this->blockSignals(isBlocked);
  return true;
}

void ModuleBase_WidgetEditor::focusTo()
{
  QPoint aPoint = QCursor::pos();

  myEditor->move(aPoint);
  myEditor->show();

  myEditor->selectAll();
  myEditor->setFocus();
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

void ModuleBase_WidgetEditor::onStopEditing()
{
  myEditor->hide();
  emit focusOutWidget(this);
}

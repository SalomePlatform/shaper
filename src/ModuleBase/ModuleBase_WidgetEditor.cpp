// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDesktopWidget>
#include <QDialog>
#include <QLayout>

// Dialog is redefined to avoid Escape key processing
class ModuleBase_EditorDialog : public QDialog
{
public:
  ModuleBase_EditorDialog(QWidget* theParent, Qt::WindowFlags theFlags)
    : QDialog(theParent, theFlags)
  {
    setObjectName("ModuleBase_EditorDialog");
    setMinimumWidth(100);
  }
  ~ModuleBase_EditorDialog() {}

protected:
  // Do nothing if key pressed because it is processing on operation manager level
  virtual void keyPressEvent(QKeyEvent* theEvent) {
    if (theEvent->key() == Qt::Key_Escape)
        return;
    QDialog::keyPressEvent(theEvent);
  }
};

ModuleBase_WidgetEditor::ModuleBase_WidgetEditor(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_WidgetDoubleValue(theParent, theData),
  myXPosition(-1), myYPosition(-1), myEditorDialog(0)
{
}

ModuleBase_WidgetEditor::~ModuleBase_WidgetEditor()
{
}

bool ModuleBase_WidgetEditor::editedValue(double theSpinMinValue, double theSpinMaxValue,
                                          double& outValue, QString& outText)
{
  bool isValueAccepted = false;

  myEditorDialog = new ModuleBase_EditorDialog(QApplication::desktop(), Qt::FramelessWindowHint);

  QHBoxLayout* aLay = new QHBoxLayout(myEditorDialog);
  aLay->setContentsMargins(2, 2, 2, 2);

  ModuleBase_ParamSpinBox* anEditor = new ModuleBase_ParamSpinBox(myEditorDialog);
  anEditor->setMinimum(theSpinMinValue);
  anEditor->setMaximum(theSpinMaxValue);

  QStringList aParameters;
  ModuleBase_Tools::getParameters(aParameters);
  anEditor->setCompletionList(aParameters);

  if (outText.isEmpty())
    anEditor->setValue(outValue);
  else
    anEditor->setText(outText);

  aLay->addWidget(anEditor);

  ModuleBase_Tools::setFocus(anEditor, "ModuleBase_WidgetEditor::editedValue");
  anEditor->selectAll();

  QPoint aPoint = QCursor::pos();
  if (myXPosition >= 0 && myYPosition >= 0)
    aPoint = QPoint(myXPosition, myYPosition);

  myEditorDialog->move(aPoint);
  isValueAccepted = myEditorDialog->exec() == QDialog::Accepted;
  if (isValueAccepted) {
    outText = anEditor->text();
    bool isDouble;
    double aValue = outText.toDouble(&isDouble);
    if (isDouble) {
      outValue = aValue;
      outText = ""; // return empty string, if it's can be converted to a double
    }
  }
  delete myEditorDialog;
  myEditorDialog = 0;
  return isValueAccepted;
}

bool ModuleBase_WidgetEditor::focusTo()
{
  showPopupEditor();
  return false;
}

bool ModuleBase_WidgetEditor::showPopupEditor(const bool theSendSignals)
{
  bool isValueAccepted = false;
  // we need to emit the focus in event manually in order to save the widget as an active
  // in the property panel before the mouse leave event happens in the viewer. The module
  // ask an active widget and change the feature visualization if the widget is not the current
  // one.  Also we need this widget as active to provide call of processEnter() applyed
  // by operation manager to the current widget. If not, the myEditorDialog will stay opened
  emitFocusInWidget();

  // nds: it seems, that the envents processing is not necessary anymore
  // White while all events will be processed
  //QApplication::processEvents();
  double aValue = mySpinBox->value();
  QString aText;
  if (mySpinBox->hasVariable())
    aText = mySpinBox->text();

  isValueAccepted = editedValue(mySpinBox->minimum(), mySpinBox->maximum(), aValue, aText);
  if (isValueAccepted) {
    if (aText.isEmpty()) {
      if (mySpinBox->hasVariable()) {
        // variable text should be cleared before setting value as the value
        // will not be set if there is a varable in control
        ModuleBase_Tools::setSpinText(mySpinBox, "");
      }
      ModuleBase_Tools::setSpinValue(mySpinBox, aValue);
    } else {
      ModuleBase_Tools::setSpinText(mySpinBox, aText);
    }
    if (theSendSignals) {
      emit valuesChanged();
      // the focus leaves the control automatically by the Enter/Esc event
      // it is processed in operation manager
      //emit focusOutWidget(this);
    }
    else
      storeValue();
  }
  ModuleBase_Tools::setFocus(mySpinBox, "ModuleBase_WidgetEditor::editedValue");
  mySpinBox->selectAll();
  // enter is processed, so we need not anymore emit clicked signal
  //if (theSendSignals && !myIsEditing && isValueAccepted)
  //  emit enterClicked(this);

  return isValueAccepted;
}

void ModuleBase_WidgetEditor::setCursorPosition(const int theX, const int theY)
{
  myXPosition = theX;
  myYPosition = theY;
}

bool ModuleBase_WidgetEditor::processEnter()
{
  if (myEditorDialog) {
    myEditorDialog->accept();
    return true;
  }

  return ModuleBase_WidgetDoubleValue::processEnter();
}

bool ModuleBase_WidgetEditor::processEscape()
{
  if (myEditorDialog) {
    myEditorDialog->reject();
    return true;
  }

  return ModuleBase_WidgetDoubleValue::processEscape();
}

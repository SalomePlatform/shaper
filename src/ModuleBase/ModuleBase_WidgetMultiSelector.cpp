/*
 * ModuleBase_WidgetMultiSelector.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>

#include <Config_WidgetAPI.h>

#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>
#include <QObject>
#include <QString>
#include <QComboBox>
#include <QEvent>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData,
                                                               const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId),
      myWorkshop(theWorkshop), myIsActive(false), myUseSubShapes(false)
{
  myMainWidget = new QWidget(theParent);
  QGridLayout* aMainLay = new QGridLayout(myMainWidget);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QLabel* aTypeLabel = new QLabel(tr("Type"), myMainWidget);
  aMainLay->addWidget(aTypeLabel, 0, 0);
  myTypeCombo = new QComboBox(myMainWidget);
  std::string aTypes = theData->getProperty("type_choice");
  myShapeTypes = QString::fromStdString(aTypes).split(' ');
  myTypeCombo->addItems(myShapeTypes);
  aMainLay->addWidget(myTypeCombo, 0, 1);
  QLabel* aListLabel = new QLabel(tr("Selected objects:"), myMainWidget);
  aMainLay->addWidget(aListLabel, 1, 0, 1, -1);
  myListControl = new QTextEdit(myMainWidget);
  myListControl->setReadOnly(true);
  aMainLay->addWidget(myListControl, 2, 0, 2, -1);
  aMainLay->setColumnStretch(1, 1);
  myMainWidget->setLayout(aMainLay);

  //TODO: Move into the base class
  myUseSubShapes = theData->getBooleanAttribute("use_subshapes", false);
  //TODO_END
  connect(myTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionTypeChanged()));
  activateSelection(true);
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
  activateSelection(false);
}

bool ModuleBase_WidgetMultiSelector::storeValue() const
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myListControl->toPlainText();
  aStringAttr->setValue(aWidgetValue.toStdString());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetMultiSelector::restoreValue()
{
  return false;
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myListControl->blockSignals(true);
  myListControl->setText(QString::fromStdString(aStringAttr->value()));
  myListControl->blockSignals(isBlocked);

  return true;
}

QWidget* ModuleBase_WidgetMultiSelector::getControl() const
{
  return myMainWidget;
}

QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  result << myTypeCombo;
  result << myListControl;
  return result;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myListControl) {
    if (theEvent->type() == QEvent::FocusIn)
      activateSelection(true);
  }
  return ModuleBase_ModelWidget::eventFilter(theObj, theEvent);
}

void ModuleBase_WidgetMultiSelector::onSelectionChanged()
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  NCollection_List<TopoDS_Shape> aSelectedShapes, aFilteredShapes;
  aSelection->selectedShapes(aSelectedShapes);
  QString aText;
  if (!aSelectedShapes.IsEmpty()) {
    filterShapes(aSelectedShapes, aFilteredShapes);
    aText = QString("Items selected: %1").arg(aFilteredShapes.Size());
  }
  myListControl->setText(aText);
}

void ModuleBase_WidgetMultiSelector::filterShapes(const NCollection_List<TopoDS_Shape>& theShapesToFilter,
                                                  NCollection_List<TopoDS_Shape>& theResult)
{
  if(myTypeCombo->count() == 0 || theShapesToFilter.IsEmpty())
    return;
  TopAbs_ShapeEnum aReferenceType =
      ModuleBase_WidgetShapeSelector::shapeType(myTypeCombo->currentText());
  NCollection_List<TopoDS_Shape>::Iterator anIter(theShapesToFilter);
  for (; anIter.More(); anIter.Next()) {
    TopoDS_Shape aShape = anIter.Value();
    if (aShape.IsNull() || aShape.ShapeType() != aReferenceType)
      continue;
    theResult.Append(aShape);
  }
}

void ModuleBase_WidgetMultiSelector::activateSelection(bool toActivate)
{
  myIsActive = toActivate;
  if (myIsActive) {
    connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    onSelectionTypeChanged();
  } else {
    disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    myWorkshop->deactivateSubShapesSelection();
  }
}

void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  QString aNewType = myTypeCombo->currentText();
  QIntList aList;
  aList.append(ModuleBase_WidgetShapeSelector::shapeType(aNewType));
  myWorkshop->activateSubShapesSelection(aList);
}

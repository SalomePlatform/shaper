// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_Definitions.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Config_WidgetAPI.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QString>
#include <QComboBox>
#include <QEvent>
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include <memory>
#include <string>

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData,
                                                               const std::string& theParentId)
    : ModuleBase_WidgetValidated(theParent, theData, theParentId),
      myWorkshop(theWorkshop), myIsActive(false)
{
  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  QLabel* aTypeLabel = new QLabel(tr("Type"), this);
  aMainLay->addWidget(aTypeLabel, 0, 0);

  myTypeCombo = new QComboBox(this);
  // There is no sence to paramerize list of types while we can not parametrize selection mode

  std::string aPropertyTypes = theData->getProperty("type_choice");
  QString aTypesStr = aPropertyTypes.c_str();
  QStringList aShapeTypes = aTypesStr.split(' ');

  myTypeCombo->addItems(aShapeTypes);
  aMainLay->addWidget(myTypeCombo, 0, 1);
  // if the xml definition contains one type, the controls to select a type should not be shown
  if (aShapeTypes.size() == 1) {
    aTypeLabel->setVisible(false);
    myTypeCombo->setVisible(false);
  }

  QLabel* aListLabel = new QLabel(tr("Selected objects:"), this);
  aMainLay->addWidget(aListLabel, 1, 0);
  // if the xml definition contains one type, an information label should be shown near to the latest
  if (aShapeTypes.size() == 1) {
    QString aLabelText = QString::fromStdString(theData->widgetLabel());
    QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
    QLabel* aSelectedLabel = new QLabel(aLabelText, this);
    if (!aLabelIcon.isEmpty())
      aSelectedLabel->setPixmap(QPixmap(aLabelIcon));
    aMainLay->addWidget(aSelectedLabel, 1, 1);
    aMainLay->setColumnStretch(2, 1);
  }

  myListControl = new QListWidget(this);
  aMainLay->addWidget(myListControl, 2, 0, 2, -1);
  aMainLay->setRowStretch(2, 1);
  aMainLay->addWidget(new QLabel(this)); //FIXME(sbh)???
  aMainLay->setRowMinimumHeight(3, 20);
  this->setLayout(aMainLay);
  connect(myTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionTypeChanged()));

  myCopyAction = new QAction(QIcon(":pictures/copy.png"), tr("Copy"), this);
  myCopyAction->setShortcut(QKeySequence::Copy);
  myCopyAction->setEnabled(false);
  connect(myCopyAction, SIGNAL(triggered(bool)), SLOT(onCopyItem()));
  myListControl->addAction(myCopyAction);
  myListControl->setContextMenuPolicy(Qt::ActionsContextMenu);
  connect(myListControl, SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
  myIsActive = false;
  activateShapeSelection();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::activateCustom()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  connect(myWorkshop, SIGNAL(selectionChanged()), 
          this,       SLOT(onSelectionChanged()), 
          Qt::UniqueConnection);

  myIsActive = true;
  activateShapeSelection();

  QObjectPtrList anObjects;
  // Restore selection in the viewer by the attribute selection list
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributeSelectionListPtr aListAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));
    if (aListAttr) {
      for (int i = 0; i < aListAttr->size(); i++) {
        AttributeSelectionPtr anAttr = aListAttr->value(i);
        ResultPtr anObject = anAttr->context();
        if (anObject.get())
          anObjects.append(anObject);
      }
    }
  }
  myWorkshop->setSelected(anObjects);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::deactivate()
{
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  myIsActive = false;
  activateShapeSelection();
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::storeValueCustom() const
{
  // the value is stored on the selection changed signal processing 
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  if (aSelectionListAttr) {
    // Store shapes type
     TopAbs_ShapeEnum aCurrentType =
           ModuleBase_Tools::shapeType(myTypeCombo->currentText());
     aSelectionListAttr->setSelectionType(myTypeCombo->currentText().toStdString());
  }   
   return true;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::restoreValue()
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  if (aSelectionListAttr) {
    // Restore shape type
    if (!aSelectionListAttr->selectionType().empty())
      setCurrentShapeType(ModuleBase_Tools::shapeType(aSelectionListAttr->selectionType().c_str()));
    updateSelectionList(aSelectionListAttr);
    return true;
  }
  return false;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::storeAttributeValue()
{
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));
  if (aSelectionListAttr.get() == NULL)
    return;

  mySelectionType = aSelectionListAttr->selectionType();
  mySelection.clear();
  int aSize = aSelectionListAttr->size();
  for (int i = 0; i < aSelectionListAttr->size(); i++) {
    AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
    mySelection.append(GeomSelection(aSelectAttr->context(), aSelectAttr->value()));
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::restoreAttributeValue(bool/* theValid*/)
{
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));
  if (aSelectionListAttr.get() == NULL)
    return;
  aSelectionListAttr->clear();

  // Store shapes type
  aSelectionListAttr->setSelectionType(mySelectionType);

  // Store selection in the attribute
  int aSize = mySelection.size();
  foreach (GeomSelection aSelec, mySelection) {
    aSelectionListAttr->append(aSelec.first, aSelec.second);
  }
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelection(const QList<ModuleBase_ViewerPrs>& theValues,
                                                  int& thePosition)
{
  if (thePosition < 0)
    return false;

  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  bool isDone = false;
  for (int i = thePosition; i < theValues.size(); i++) {
    ModuleBase_ViewerPrs aValue = theValues[i];
    thePosition++;
    bool aProcessed = false;
    if (isValidSelection(aValue)) {
      aProcessed = setSelectionCustom(aValue);
    }
    // if there is at least one set, the result is true
    isDone = isDone || aProcessed;
    // when an object, which do not satisfy the validating process, stop set selection
    if (!aProcessed)
      break;
  }
  if (isDone) {
    updateObject(myFeature);
    emit valuesChanged();
  }
  return isDone;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  TopoDS_Shape aShape = thePrs.shape();
  if ((myTypeCombo->count() > 1) && (!aShape.IsNull())) {
    TopAbs_ShapeEnum aType = ModuleBase_Tools::shapeType(myTypeCombo->currentText());
    if (aShape.ShapeType() != aType)
      return false;
  }
  ResultPtr aResult;
  if (!thePrs.owner().IsNull()) {
    ObjectPtr anObject = myWorkshop->selection()->getSelectableObject(thePrs.owner());
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  }
  else {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs.object());
  }


  if (myFeature) {
    // We can not select a result of our feature
    const std::list<ResultPtr>& aResList = myFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    bool isSkipSelf = false;
    for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
      if ((*aIt) == aResult) {
        isSkipSelf = true;
        break;
      }
    }
    if(isSkipSelf)
      return false;
  }

  // if the result has the similar shap as the parameter shape, just the context is set to the
  // selection list attribute.
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  const TopoDS_Shape& aTDSShape = thePrs.shape();
  // if only result is selected, an empty shape is set to the model
  if (aTDSShape.IsNull()) {
    aSelectionListAttr->append(aResult, GeomShapePtr());
  }
  else {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aTDSShape));
    // We can not select a result of our feature
    if (aShape->isEqual(aResult->shape()))
      aSelectionListAttr->append(aResult, GeomShapePtr());
    else
      aSelectionListAttr->append(aResult, aShape);
  }
  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  //result << myTypeCombo;
  result << myListControl;
  return result;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  //TODO: Remove maybe?
  return ModuleBase_ModelWidget::eventFilter(theObj, theEvent);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  activateShapeSelection();
  QObjectPtrList anEmptyList;
  // This method will call Selection changed event which will call onSelectionChanged
  // To clear mySelection, myListControl and storeValue()
  // So, we don't need to call it
  myWorkshop->setSelected(anEmptyList);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrs> aSelected = getSelectedEntitiesOrObjects(myWorkshop->selection());

  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  aSelectionListAttr->clear();
  if (aSelected.size() > 0) {
    foreach (ModuleBase_ViewerPrs aPrs, aSelected) {
      if (isValidSelection(aPrs)) {
        setSelectionCustom(aPrs);
      }
    }
  }
  emit valuesChanged();
  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const TopAbs_ShapeEnum theShapeType)
{
  QString aShapeTypeName;
  
  for (int idx = 0; idx < myTypeCombo->count(); ++idx) {
    aShapeTypeName = myTypeCombo->itemText(idx);
    TopAbs_ShapeEnum aRefType = ModuleBase_Tools::shapeType(aShapeTypeName);
    if(aRefType == theShapeType && idx != myTypeCombo->currentIndex()) {
      myIsActive = false;
      activateShapeSelection();
      bool isBlocked = myTypeCombo->blockSignals(true);
      myTypeCombo->setCurrentIndex(idx);
      myIsActive = true;
      myTypeCombo->blockSignals(isBlocked);
      activateShapeSelection();
      break;
    }
  }
}

void ModuleBase_WidgetMultiSelector::activateShapeSelection()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  if (myIsActive) {
    QString aNewType = myTypeCombo->currentText();
    QIntList aList;
    aList.append(ModuleBase_Tools::shapeType(aNewType));
    myWorkshop->activateSubShapesSelection(aList);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }

  activateFilters(myWorkshop, myIsActive);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionList(AttributeSelectionListPtr theList)
{
  myListControl->clear();
  for (int i = 0; i < theList->size(); i++) {
    AttributeSelectionPtr aAttr = theList->value(i);
    myListControl->addItem(aAttr->namingName().c_str());
  }
  // We have to call repaint because sometimes the List control is not updated
  myListControl->repaint();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onCopyItem()
{
  QList<QListWidgetItem*> aItems = myListControl->selectedItems();
  QString aRes;
  foreach(QListWidgetItem* aItem, aItems) {
    if (!aRes.isEmpty())
      aRes += "\n";
    aRes += aItem->text();
  }
  if (!aRes.isEmpty()) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(aRes);
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onListSelection()
{
  QList<QListWidgetItem*> aItems = myListControl->selectedItems();
  myCopyAction->setEnabled(!aItems.isEmpty());
}


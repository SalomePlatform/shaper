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

#include <GeomValidators_ShapeType.h>

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
      myWorkshop(theWorkshop)
{
  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  QLabel* aTypeLabel = new QLabel(tr("Type"), this);
  aMainLay->addWidget(aTypeLabel, 0, 0);

  myTypeCombo = new QComboBox(this);
  // There is no sence to paramerize list of types while we can not parametrize selection mode

  myShapeValidator = new GeomValidators_ShapeType();

  std::string aPropertyTypes = theData->getProperty("type_choice");
  QString aTypesStr = aPropertyTypes.c_str();
  QStringList aShapeTypes = aTypesStr.split(' ');

  //myIsUseChoice = theData->getBooleanAttribute("use_choice", true);

  myTypeCombo->addItems(aShapeTypes);
  aMainLay->addWidget(myTypeCombo, 0, 1);
  // if the xml definition contains one type, the controls to select a type should not be shown
  if (aShapeTypes.size() == 1/* || !myIsUseChoice*/) {
    aTypeLabel->setVisible(false);
    myTypeCombo->setVisible(false);
  }

// Modification for specification of 1.3.0
  std::string aLabelText = "";//theData->getProperty("label");
  QLabel* aListLabel = new QLabel(!aLabelText.empty() ? aLabelText.c_str()
                                                      : tr("Selected objects:"), this);
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
  delete myShapeValidator;
}

//TODO: nds stabilization hotfix
void ModuleBase_WidgetMultiSelector::disconnectSignals()
{
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::activateCustom()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  connect(myWorkshop, SIGNAL(selectionChanged()), 
          this,       SLOT(onSelectionChanged()), 
          Qt::UniqueConnection);

  activateShapeSelection(true);

  // Restore selection in the viewer by the attribute selection list
  myWorkshop->setSelected(getAttributeSelection());

  activateFilters(myWorkshop, true);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::deactivate()
{
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateShapeSelection(false);
  activateFilters(myWorkshop, false);
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
void ModuleBase_WidgetMultiSelector::customValidators(
                                        std::list<ModelAPI_Validator*>& theValidators,
                                        std::list<std::list<std::string> >& theArguments) const
{
  return;
  std::list<std::string> anArguments;

  theValidators.push_back(myShapeValidator);
  if (true/*myIsUseChoice*/) {
    QString aType = myTypeCombo->currentText();
    anArguments.push_back(validatorType(aType));
  }
  else {
    for(int i = 0, aCount = myTypeCombo->count(); i < aCount; i++) {
      anArguments.push_back(validatorType(myTypeCombo->itemText(i)));
    }
  }
  theArguments.push_back(anArguments);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::acceptSubShape(const TopoDS_Shape& theShape) const
{
  bool aValid = true;
  if (theShape.IsNull()) {
    aValid = true; // do not check the shape type if the shape is empty
    // extrusion uses a sketch object selectected in Object browser
  }
  else {
    aValid = false;
    TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
    if (myTypeCombo->count() > 1) {
      TopAbs_ShapeEnum aType = ModuleBase_Tools::shapeType(myTypeCombo->currentText());
      aValid = aShapeType == aType;
    }
    else {
      for(int i = 0, aCount = myTypeCombo->count(); i < aCount && !aValid; i++) {
        TopAbs_ShapeEnum aType = ModuleBase_Tools::shapeType(myTypeCombo->itemText(i));
        aValid = aShapeType == aType;
      }
    }
  }
  return aValid;
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
    bool aProcessed = false;
    if (isValidSelection(aValue)) {
      aProcessed = setSelectionCustom(aValue);
    }
    // if there is at least one set, the result is true
    isDone = isDone || aProcessed;
    // when an object, which do not satisfy the validating process, stop set selection
    if (!aProcessed)
      break;
    else
      thePosition++;
  }
  if (isDone) {
    updateObject(myFeature);
    // this emit is necessary to call store/restore method an restore type of selection
    emit valuesChanged();
  }
  return isDone;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  TopoDS_Shape aShape = thePrs.shape();
  if (!acceptSubShape(aShape))
    return false;

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
  activateShapeSelection(true);
  activateFilters(myWorkshop, true);
  QList<ModuleBase_ViewerPrs> anEmptyList;
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

  // Set focus to List control in order to make possible 
  // to use Tab key for transfer the focus to next widgets
  myListControl->setCurrentRow(myListControl->model()->rowCount() - 1);
  myListControl->setFocus();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const TopAbs_ShapeEnum theShapeType)
{
  QString aShapeTypeName;
  
  for (int idx = 0; idx < myTypeCombo->count(); ++idx) {
    aShapeTypeName = myTypeCombo->itemText(idx);
    TopAbs_ShapeEnum aRefType = ModuleBase_Tools::shapeType(aShapeTypeName);
    if(aRefType == theShapeType && idx != myTypeCombo->currentIndex()) {
      activateShapeSelection(false);
      activateFilters(myWorkshop, false);
      bool isBlocked = myTypeCombo->blockSignals(true);
      myTypeCombo->setCurrentIndex(idx);
      myTypeCombo->blockSignals(isBlocked);

      activateShapeSelection(true);
      activateFilters(myWorkshop, true);
      break;
    }
  }
}

void ModuleBase_WidgetMultiSelector::activateShapeSelection(const bool isActivated)
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  if (isActivated) {
    QString aNewType = myTypeCombo->currentText();
    QIntList aList;
    if (true /*myIsUseChoice*/) {
      aList.append(ModuleBase_Tools::shapeType(aNewType));
    }
    else {
      for(int i = 0, aCount = myTypeCombo->count(); i < aCount; i++)
        aList.append(ModuleBase_Tools::shapeType(myTypeCombo->itemText(i)));
    }
    myWorkshop->activateSubShapesSelection(aList);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
}

QList<ModuleBase_ViewerPrs> ModuleBase_WidgetMultiSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrs> aSelected;
  // Restore selection in the viewer by the attribute selection list
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributeSelectionListPtr aListAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));
    if (aListAttr) {
      for (int i = 0; i < aListAttr->size(); i++) {
        AttributeSelectionPtr anAttr = aListAttr->value(i);
        ResultPtr anObject = anAttr->context();
        if (anObject.get()) {
          TopoDS_Shape aShape;
          std::shared_ptr<GeomAPI_Shape> aShapePtr = anAttr->value();
          if (aShapePtr.get()) {
            aShape = aShapePtr->impl<TopoDS_Shape>();
          }
          aSelected.append(ModuleBase_ViewerPrs(anObject, aShape, NULL));
        }
      }
    }
  }
  return aSelected;
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
std::string ModuleBase_WidgetMultiSelector::validatorType(const QString& theType) const
{
  std::string aType;

  if (theType == "Vertices")
    aType = "vertex";
  else if (theType == "Edges")
    aType = "edge";
  else if (theType == "Faces")
    aType = "face";
  else if (theType == "Solids")
    aType = "solid";

  return aType;
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


// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetMultiSelector.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_FilterNoDegeneratedEdge.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>

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
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  if (aSelectionListAttr) {
    aSelectionListAttr->clear();
    // Store shapes type
    TopAbs_ShapeEnum aCurrentType =
          ModuleBase_WidgetShapeSelector::shapeType(myTypeCombo->currentText());
    aSelectionListAttr->setSelectionType(myTypeCombo->currentText().toStdString());
    // Store selection in the attribute
    foreach (GeomSelection aSelec, mySelection) {
      aSelectionListAttr->append(aSelec.first, aSelec.second);
    }
    //updateSelectionList(aSelectionListAttr);
    updateObject(myFeature);
    return true;
  }
  return false;
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
    mySelection.clear();
    // Restore shape type
    setCurrentShapeType(
      ModuleBase_WidgetShapeSelector::shapeType(aSelectionListAttr->selectionType().c_str()));
    // Restore selection in the list
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      mySelection.append(GeomSelection(aSelectAttr->context(), aSelectAttr->value()));
    }
    updateSelectionList(aSelectionListAttr);
    return true;
  }
  return false;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::backupAttributeValue(const bool isBackup)
{

}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelection(const Handle_SelectMgr_EntityOwner& theOwner)
{
  return false;
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
  myWorkshop->setSelected(anEmptyList);
  // Clear mySelection, myListControl and storeValue()
  onSelectionChanged();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionChanged()
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  NCollection_List<TopoDS_Shape> aSelectedShapes;
  std::list<ObjectPtr> aOwnersList;
  aSelection->selectedShapes(aSelectedShapes, aOwnersList);

  mySelection.clear();
  std::list<ObjectPtr>::const_iterator aIt;
  NCollection_List<TopoDS_Shape>::Iterator aShpIt(aSelectedShapes);
  GeomShapePtr aShape;
  for (aIt = aOwnersList.cbegin(); aIt != aOwnersList.cend(); aShpIt.Next(), aIt++) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(*aIt);
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
        continue;
    }
    aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aShpIt.Value()));

    // if the result has the similar shap as the parameter shape, just the context is set to the
    // selection list attribute.
    if (aShape->isEqual(aResult->shape()))
      mySelection.append(GeomSelection(aResult, NULL));
    else
      mySelection.append(GeomSelection(aResult, aShape));
  }
  //updateSelectionList();
  emit valuesChanged();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const TopAbs_ShapeEnum theShapeType)
{
  QString aShapeTypeName;
  
  for (int idx = 0; idx < myTypeCombo->count(); ++idx) {
    aShapeTypeName = myTypeCombo->itemText(idx);
    TopAbs_ShapeEnum aRefType = ModuleBase_WidgetShapeSelector::shapeType(aShapeTypeName);
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
    aList.append(ModuleBase_WidgetShapeSelector::shapeType(aNewType));
    myWorkshop->activateSubShapesSelection(aList);

    // it is necessary to filter the selected edges to be non-degenerated
    // it is not possible to build naming name for such edges
    if (aNewType == "Edges") {
      myEdgesTypeFilter = new ModuleBase_FilterNoDegeneratedEdge();
      aViewer->addSelectionFilter(myEdgesTypeFilter);
    }
    else {
      aViewer->removeSelectionFilter(myEdgesTypeFilter);
    }

  } else {
    myWorkshop->deactivateSubShapesSelection();
    aViewer->removeSelectionFilter(myEdgesTypeFilter);
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


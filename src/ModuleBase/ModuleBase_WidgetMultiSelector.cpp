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
#include <ModuleBase_IModule.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IconFactory.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Tools.h>

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
#include <QTimer>

#include <memory>
#include <string>

const int ATTRIBUTE_SELECTION_INDEX_ROLE = Qt::UserRole + 1;

/**
* Customization of a List Widget to make it to be placed on full width of container
*/
class CustomListWidget : public QListWidget
{
public:
  /// Constructor
  /// \param theParent a parent widget
  CustomListWidget( QWidget* theParent )
    : QListWidget( theParent )
  {
  }

  /// Redefinition of virtual method
  virtual QSize	sizeHint() const
  {
    int aHeight = 2*QFontMetrics( font() ).height();
    QSize aSize = QListWidget::sizeHint();
    return QSize( aSize.width(), aHeight );
  }

  /// Redefinition of virtual method
  virtual QSize	minimumSizeHint() const
  {
    int aHeight = 4/*2*/*QFontMetrics( font() ).height();
    QSize aSize = QListWidget::minimumSizeHint();
    return QSize( aSize.width(), aHeight );
  }

#ifndef WIN32
// The code is necessary only for Linux because
//it can not update viewport on widget resize
protected:
  void resizeEvent(QResizeEvent* theEvent)
  {
    QListWidget::resizeEvent(theEvent);
    QTimer::singleShot(5, viewport(), SLOT(repaint()));
  }
#endif
};

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theWorkshop, theData)
{
  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  QLabel* aTypeLabel = new QLabel(tr("Type"), this);
  aMainLay->addWidget(aTypeLabel, 0, 0);

  myTypeCombo = new QComboBox(this);
  // There is no sense to parameterize list of types while we can not parameterize selection mode

  std::string aPropertyTypes = theData->getProperty("type_choice");
  QString aTypesStr = aPropertyTypes.c_str();
  QStringList aShapeTypes = aTypesStr.split(' ', QString::SkipEmptyParts);

  myIsUseChoice = theData->getBooleanAttribute("use_choice", false);

  if (!aShapeTypes.empty())
    myTypeCombo->addItems(aShapeTypes);
  aMainLay->addWidget(myTypeCombo, 0, 1);
  // if the xml definition contains one type, the controls to select a type should not be shown
  if (aShapeTypes.size() <= 1 || !myIsUseChoice) {
    aTypeLabel->setVisible(false);
    myTypeCombo->setVisible(false);
  }

  std::string aLabelText = theData->getProperty("label");
  QLabel* aListLabel = new QLabel(!aLabelText.empty() ? aLabelText.c_str()
                                                      : tr("Selected objects:"), this);
  aMainLay->addWidget(aListLabel, 1, 0);
  // if the xml definition contains one type, an information label should be shown near to the latest
  if (aShapeTypes.size() <= 1) {
    QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
    if (!aLabelIcon.isEmpty()) {
      QLabel* aSelectedLabel = new QLabel("", this);
      aSelectedLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
      aMainLay->addWidget(aSelectedLabel, 1, 1);
    }
    aMainLay->setColumnStretch(2, 1);
  }

  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myListControl = new CustomListWidget(this);
  QString anObjName = QString::fromStdString(attributeID());
  myListControl->setObjectName(anObjName);
  myListControl->setToolTip(aToolTip);
  myListControl->setSelectionMode(QAbstractItemView::ExtendedSelection);

  aMainLay->addWidget(myListControl, 2, 0, 1, -1);
  aMainLay->setRowStretch(2, 1);
  //aMainLay->addWidget(new QLabel(this)); //FIXME(sbh)???
  //aMainLay->setRowMinimumHeight(3, 20);
  //this->setLayout(aMainLay);
  connect(myTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionTypeChanged()));

  myCopyAction = new QAction(QIcon(":pictures/copy.png"), tr("Copy"), this);
  myCopyAction->setShortcut(QKeySequence::Copy);
  myCopyAction->setEnabled(false);
  connect(myCopyAction, SIGNAL(triggered(bool)), SLOT(onCopyItem()));
  myListControl->addAction(myCopyAction);

  myDeleteAction = new QAction(QIcon(":pictures/delete.png"), tr("Delete"), this);
  myDeleteAction->setEnabled(false);
  connect(myDeleteAction, SIGNAL(triggered(bool)), SLOT(onDeleteItem()));
  myListControl->addAction(myDeleteAction);

  myListControl->setContextMenuPolicy(Qt::ActionsContextMenu);
  connect(myListControl, SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::activateCustom()
{
  ModuleBase_WidgetSelector::activateCustom();

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::deactivate()
{
  ModuleBase_WidgetSelector::deactivate();

  myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::storeValueCustom()
{
  // the value is stored on the selection changed signal processing 
  // A rare case when plugin was not loaded. 
  if (!myFeature)
    return false;

  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    aSelectionListAttr->setSelectionType(myTypeCombo->currentText().toStdString());
  }
  return true;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::restoreValueCustom()
{
  // A rare case when plugin was not loaded. 
  if (!myFeature)
    return false;

  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    // Restore shape type
    std::string aSelectionType = aSelectionListAttr->selectionType().c_str();
    if (!aSelectionType.empty())
      setCurrentShapeType(ModuleBase_Tools::shapeType(aSelectionType.c_str()));
  }
  updateSelectionList();
  return true;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                                  const bool theToValidate)
{
  AttributeSelectionListPtr aSelectionListAttr;
  if (attribute()->attributeType() == ModelAPI_AttributeSelectionList::typeId())
    aSelectionListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(attribute());
  if (aSelectionListAttr.get())
    aSelectionListAttr->cashValues(true);

  /// remove unused objects from the model attribute.
  /// It should be performed before new attributes append.
  removeUnusedAttributeObjects(theValues);

  QList<ModuleBase_ViewerPrsPtr> anInvalidValues;
  QList<ModuleBase_ViewerPrsPtr> anAttributeValues;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aValue = *anIt;
    // do not validate and append to attribute selection presentation if it exists in the attribute
    ObjectPtr anObject;
    GeomShapePtr aShape;
    getGeomSelection(aValue, anObject, aShape);
    if (ModuleBase_Tools::hasObject(attribute(), anObject, aShape, myWorkshop, myIsInValidate)) {
      anAttributeValues.append(aValue);
      continue;
    }
    if (theToValidate && !isValidInFilters(aValue))
      anInvalidValues.append(aValue);
  }
  bool aHasInvalidValues = anInvalidValues.size() > 0;

  bool isDone = false;
  for (anIt = theValues.begin(); anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aValue = *anIt;
    bool aProcessed = false;
    if ((aHasInvalidValues && anInvalidValues.contains(aValue)) ||
        anAttributeValues.contains(aValue))
      continue;
    aProcessed = setSelectionCustom(aValue); /// it is not optimal as hasObject() is already checked
    // if there is at least one set, the result is true
    isDone = isDone || aProcessed;
  }
  // updateObject - to update/redisplay feature
  // it is commented in order to perfom it outside the method
  //if (isDone) {
    //updateObject(myFeature);
    // this emit is necessary to call store/restore method an restore type of selection
    //emit valuesChanged();
  //}

  if (aSelectionListAttr.get())
    aSelectionListAttr->cashValues(false);

  theValues.clear();
  if (!anInvalidValues.empty())
    theValues.append(anInvalidValues);

  return isDone;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::getHighlighted(QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  std::set<int> anAttributeIds;
  getSelectedAttributeIndices(anAttributeIds);
  if (!anAttributeIds.empty())
    convertIndicesToViewerSelection(anAttributeIds, theValues);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = ModuleBase_WidgetSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
    aValid = aResult.get() != NULL;
    if (aValid) {
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
        if (isSkipSelf)
          aValid = false;
      }
    }
  }
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::processDelete()
{
  // find attribute indices to delete
  std::set<int> anAttributeIds;
  getSelectedAttributeIndices(anAttributeIds);

  // refill attribute by the items which indices are not in the list of ids
  bool aDone = false;
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  aDone = !anAttributeIds.empty();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    aSelectionListAttr->remove(anAttributeIds);

  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    aRefListAttr->remove(anAttributeIds);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    aRefAttrListAttr->remove(anAttributeIds);
  }

  if (aDone) {
    // update object is necessary to flush update signal. It leads to objects references map update
    // and the operation presentation will not contain deleted items visualized as parameters of
    // the feature.
    updateObject(myFeature);

    restoreValue();
    myWorkshop->setSelected(getAttributeSelection());
  }
  return aDone;
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
void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  activateSelectionAndFilters(true);
  QList<ModuleBase_ViewerPrsPtr> anEmptyList;
  // This method will call Selection changed event which will call onSelectionChanged
  // To clear mySelection, myListControl and storeValue()
  // So, we don't need to call it
  myWorkshop->setSelected(anEmptyList);
}

void ModuleBase_WidgetMultiSelector::updateFocus()
{
  // Set focus to List control in order to make possible 
  // to use Tab key for transfer the focus to next widgets
  myListControl->setCurrentRow(myListControl->model()->rowCount() - 1);
  ModuleBase_Tools::setFocus(myListControl,
                             "ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()");
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionName()
{
}

//********************************************************************
QIntList ModuleBase_WidgetMultiSelector::getShapeTypes() const
{
  QIntList aShapeTypes;

  if (myTypeCombo->count() > 1 && myIsUseChoice) {
    aShapeTypes.append(ModuleBase_Tools::shapeType(myTypeCombo->currentText()));
  }
  else {
    for (int i = 0, aCount = myTypeCombo->count(); i < aCount; i++) {
      TopAbs_ShapeEnum aType = ModuleBase_Tools::shapeType(myTypeCombo->itemText(i));
      aShapeTypes.append(aType);
      if (aType == TopAbs_SOLID)
        aShapeTypes.append(TopAbs_COMPSOLID);
    }
  }
  return aShapeTypes;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const TopAbs_ShapeEnum theShapeType)
{
  QString aShapeTypeName;
  
  for (int idx = 0; idx < myTypeCombo->count(); ++idx) {
    aShapeTypeName = myTypeCombo->itemText(idx);
    TopAbs_ShapeEnum aRefType = ModuleBase_Tools::shapeType(aShapeTypeName);
    if(aRefType == theShapeType && idx != myTypeCombo->currentIndex()) {
      bool aWasActivated = activateSelectionAndFilters(false);
      bool isBlocked = myTypeCombo->blockSignals(true);
      myTypeCombo->setCurrentIndex(idx);
      myTypeCombo->blockSignals(isBlocked);
      if (aWasActivated)
        activateSelectionAndFilters(true);
      break;
    }
  }
}

QList<ModuleBase_ViewerPrsPtr> ModuleBase_WidgetMultiSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrsPtr> aSelected;
  convertIndicesToViewerSelection(std::set<int>(), aSelected);
  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionList()
{
  myListControl->clear();

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aAttr = aSelectionListAttr->value(i);
      QListWidgetItem* anItem = new QListWidgetItem(aAttr->namingName().c_str(), myListControl);
      anItem->setData(ATTRIBUTE_SELECTION_INDEX_ROLE, i);
      myListControl->addItem(anItem);
    }
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        QListWidgetItem* anItem = new QListWidgetItem(anObject->data()->name().c_str(),
                                                      myListControl);
        anItem->setData(ATTRIBUTE_SELECTION_INDEX_ROLE, i);
        myListControl->addItem(anItem);
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
      QString aName;
      if (anAttribute.get()) {
        std::string anAttrName = generateName(anAttribute, myWorkshop);
        aName = QString::fromStdString(anAttrName);
      }
      else {
        ObjectPtr anObject = aRefAttrListAttr->object(i);
        if (anObject.get()) {
          aName = anObject->data()->name().c_str();
        }
      }
      QListWidgetItem* anItem = new QListWidgetItem(aName, myListControl);
      anItem->setData(ATTRIBUTE_SELECTION_INDEX_ROLE, i);
      myListControl->addItem(anItem);
    }
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
void ModuleBase_WidgetMultiSelector::onDeleteItem()
{
  processDelete();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onListSelection()
{
  QList<QListWidgetItem*> aItems = myListControl->selectedItems();
  myCopyAction->setEnabled(!aItems.isEmpty());
  myDeleteAction->setEnabled(!aItems.isEmpty());
  
  myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeHighlightedObjects,
                                        true);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::getSelectedAttributeIndices(std::set<int>& theAttributeIds)
{
  QList<QListWidgetItem*> aItems = myListControl->selectedItems();
  foreach(QListWidgetItem* anItem, aItems) {
    int anIndex = anItem->data(ATTRIBUTE_SELECTION_INDEX_ROLE).toInt();
    if (theAttributeIds.find(anIndex) == theAttributeIds.end())
      theAttributeIds.insert(anIndex);
  }
}

void ModuleBase_WidgetMultiSelector::convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                                                      QList<ModuleBase_ViewerPrsPtr>& theValues) const
{
  if(myFeature.get() == NULL)
    return;

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
      ResultPtr anObject = anAttr->context();
      if (anObject.get())
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, anAttr->value(), NULL)));
    }
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      ObjectPtr anObject = aRefAttrListAttr->object(i);
      if (!anObject.get())
        continue;
      TopoDS_Shape aShape;
      AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
      if (anAttribute.get()) {
        GeomShapePtr aGeomShape = ModuleBase_Tools::getShape(anAttribute, myWorkshop);
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, aGeomShape, NULL)));
      }
    }
  }
}

void ModuleBase_WidgetMultiSelector::removeUnusedAttributeObjects
                                                 (QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  std::map<ObjectPtr, std::set<GeomShapePtr> > aGeomSelection = convertSelection(theValues);
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  std::set<GeomShapePtr> aShapes;
  std::set<int> anIndicesToBeRemoved;
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    // iteration through data model to find not selected elements to remove them
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
      bool aFound = findInSelection(anAttr->context(), anAttr->value(), aGeomSelection);
      if (!aFound)
        anIndicesToBeRemoved.insert(i);
    }
    aSelectionListAttr->remove(anIndicesToBeRemoved);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        bool aFound = findInSelection(anObject, GeomShapePtr(), aGeomSelection);
        if (!aFound)
          anIndicesToBeRemoved.insert(i);
      }
    }
    aRefListAttr->remove(anIndicesToBeRemoved);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    std::set<AttributePtr> anAttributes;
    QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
    ObjectPtr anObject;
    GeomShapePtr aShape;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrsPtr aPrs = *anIt;
      getGeomSelection(aPrs, anObject, aShape);
      AttributePtr anAttr = myWorkshop->module()->findAttribute(anObject, aShape);
      if (anAttr.get() && anAttributes.find(anAttr) == anAttributes.end())
        anAttributes.insert(anAttr);
    }

    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      bool aFound = false;
      if (aRefAttrListAttr->isAttribute(i)) {
        AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
        aFound = anAttributes.find(anAttribute) != anAttributes.end();
      }
      else {
        aFound = findInSelection(aRefAttrListAttr->object(i), GeomShapePtr(), aGeomSelection);
      }
      if (!aFound)
        anIndicesToBeRemoved.insert(i);
    }
    aRefAttrListAttr->remove(anIndicesToBeRemoved);
  }
}

std::map<ObjectPtr, std::set<GeomShapePtr> > ModuleBase_WidgetMultiSelector::convertSelection
                                                     (QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  // convert prs list to objects map
  std::map<ObjectPtr, std::set<GeomShapePtr> > aGeomSelection;
  std::set<GeomShapePtr> aShapes;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  ObjectPtr anObject;
  GeomShapePtr aShape;
  GeomShapePtr anEmptyShape(new GeomAPI_Shape());
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = *anIt;
    getGeomSelection(aPrs, anObject, aShape);
    aShapes.clear();
    if (aGeomSelection.find(anObject) != aGeomSelection.end()) // found
      aShapes = aGeomSelection[anObject];
    // we need to know if there was an empty shape in selection for the object
    if (!aShape.get())
      aShape = anEmptyShape;
    if (aShape.get() && aShapes.find(aShape) == aShapes.end()) // not found
      aShapes.insert(aShape);
    aGeomSelection[anObject] = aShapes;
  }
  return aGeomSelection;
}

bool ModuleBase_WidgetMultiSelector::findInSelection(const ObjectPtr& theObject,
                              const GeomShapePtr& theShape,
                              const std::map<ObjectPtr, std::set<GeomShapePtr> >& theGeomSelection)
{
  bool aFound = false;
  GeomShapePtr anEmptyShape(new GeomAPI_Shape());
  GeomShapePtr aShape = theShape.get() ? theShape : anEmptyShape;
  if (theGeomSelection.find(theObject) != theGeomSelection.end()) {// found
    const std::set<GeomShapePtr>& aShapes = theGeomSelection.at(theObject);
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast && !aFound; anIt++) {
      GeomShapePtr aCShape = *anIt;
      if (aCShape.get())
        aFound = aCShape->isEqual(aShape);
    }
  }
  return aFound;
}

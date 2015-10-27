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
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>

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
    int aHeight = 2*QFontMetrics( font() ).height();
    QSize aSize = QListWidget::minimumSizeHint();
    return QSize( aSize.width(), aHeight );
  }
};

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData,
                                                               const std::string& theParentId)
 : ModuleBase_WidgetSelector(theParent, theWorkshop, theData, theParentId),
   mySelectionCount(0)
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

  myIsUseChoice = theData->getBooleanAttribute("use_choice", true);

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
      aSelectedLabel->setPixmap(QPixmap(aLabelIcon));
      aMainLay->addWidget(aSelectedLabel, 1, 1);
    }
    aMainLay->setColumnStretch(2, 1);
  }

  myListControl = new CustomListWidget(this);
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
  myListControl->setContextMenuPolicy(Qt::ActionsContextMenu);
  connect(myListControl, SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::storeValueCustom() const
{
  // the value is stored on the selection changed signal processing 
  // A rare case when plugin was not loaded. 
  if (!myFeature)
    return false;

  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
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

  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
    // Restore shape type
    if (!aSelectionListAttr->selectionType().empty())
      setCurrentShapeType(ModuleBase_Tools::shapeType(aSelectionListAttr->selectionType().c_str()));
  }
  updateSelectionList();
  return true;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::storeAttributeValue()
{
  ModuleBase_WidgetValidated::storeAttributeValue();

  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
    mySelectionType = aSelectionListAttr->selectionType();
    mySelectionCount = aSelectionListAttr->size();
  }
  else {
    AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
    mySelectionCount = aRefListAttr->size();
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::restoreAttributeValue(bool theValid)
{
  ModuleBase_WidgetValidated::restoreAttributeValue(theValid);

  // Store shape type
  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
    aSelectionListAttr->setSelectionType(mySelectionType);

    // restore selection in the attribute. Indeed there is only one stored object
    int aCountAppened = aSelectionListAttr->size() - mySelectionCount;
    for (int i = 0; i < aCountAppened; i++)
      aSelectionListAttr->removeLast();
  }
  else {
    AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
    // restore objects in the attribute. Indeed there is only one stored object
    int aCountAppened = aRefListAttr->size() - mySelectionCount;
    for (int i = 0; i < aCountAppened; i++)
      aRefListAttr->removeLast();
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::clearAttribute()
{
  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get())
    aSelectionListAttr->clear();
  else {
    AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
    aRefListAttr->clear();
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setObject(ObjectPtr theSelectedObject,
                                               GeomShapePtr theShape)
{
  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
    aSelectionListAttr->append(aResult, theShape, myIsInValidate);
  }
  else {
    AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
    aRefListAttr->append(theSelectedObject);
  }
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                                  const bool theToValidate)
{
  QList<ModuleBase_ViewerPrs> aSkippedValues;

  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  bool isDone = false;
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrs aValue = *anIt;
    bool aProcessed = false;
    if (!theToValidate || isValidInFilters(aValue)) {
      aProcessed = setSelectionCustom(aValue);
    }
    else
      aSkippedValues.append(aValue);
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
  theValues.clear();
  if (!aSkippedValues.empty())
    theValues.append(aSkippedValues);

  return isDone;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
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
  QList<ModuleBase_ViewerPrs> anEmptyList;
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
  myListControl->setFocus();
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
      activateSelectionAndFilters(false);
      bool isBlocked = myTypeCombo->blockSignals(true);
      myTypeCombo->setCurrentIndex(idx);
      myTypeCombo->blockSignals(isBlocked);

      activateSelectionAndFilters(true);
      break;
    }
  }
}

QList<ModuleBase_ViewerPrs> ModuleBase_WidgetMultiSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrs> aSelected;
  // Restore selection in the viewer by the attribute selection list
  if(myFeature) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    if (aSelectionListAttr.get()) {
      for (int i = 0; i < aSelectionListAttr->size(); i++) {
        AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
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
    else {
      AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
      if (aRefListAttr.get()) {
        for (int i = 0; i < aRefListAttr->size(); i++) {
          ObjectPtr anObject = aRefListAttr->object(i);
          if (anObject.get()) {
            aSelected.append(ModuleBase_ViewerPrs(anObject, TopoDS_Shape(), NULL));
          }
        }
      }
    }
  }
  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionList()
{
  myListControl->clear();

  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  if (aSelectionListAttr.get()) {
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aAttr = aSelectionListAttr->value(i);
      myListControl->addItem(aAttr->namingName().c_str());
    }
  }
  else {
    AttributeRefListPtr aRefListAttr = myFeature->data()->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get())
        myListControl->addItem(anObject->data()->name().c_str());
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
void ModuleBase_WidgetMultiSelector::onListSelection()
{
  QList<QListWidgetItem*> aItems = myListControl->selectedItems();
  myCopyAction->setEnabled(!aItems.isEmpty());
}

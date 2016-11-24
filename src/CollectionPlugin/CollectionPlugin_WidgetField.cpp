// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetField.cpp
// Created:     16 Nov 2016
// Author:      Vitaly SMETANNIKOV

#include "CollectionPlugin_WidgetField.h"
#include "CollectionPlugin_Field.h"

#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeIntArray.h>

#include <QLayout>
#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QStackedWidget>
#include <QValidator>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>

const char* MYFirstCol = "Shape";
const char* MYTrue = "True";
const char* MYFalse = "False";

class DataTableItemDelegate : public QStyledItemDelegate
{
public:
  DataTableItemDelegate(ModelAPI_AttributeTables::ValueType theType) : 
      QStyledItemDelegate() { myType = theType; }

  virtual QWidget* createEditor(QWidget* theParent, 
                                const QStyleOptionViewItem & theOption, 
                                const QModelIndex& theIndex) const;

  ModelAPI_AttributeTables::ValueType dataType() const { return myType; }

  void setDataType(ModelAPI_AttributeTables::ValueType theType) { myType = theType; }
   
signals:
  void startEditing();

private:
  ModelAPI_AttributeTables::ValueType myType;
};

QWidget* DataTableItemDelegate::createEditor(QWidget* theParent, 
                                             const QStyleOptionViewItem & theOption, 
                                             const QModelIndex& theIndex ) const
{
  if ((theIndex.column() == 0) && (theIndex.row() > 0)) {
    QWidget* aWgt = QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
    QLineEdit* aEdt = static_cast<QLineEdit*>(aWgt);
    aEdt->setReadOnly(true);
    return aEdt;
  } else {
    QLineEdit* aLineEdt = 0;
    switch (myType) {
    case ModelAPI_AttributeTables::DOUBLE:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent, 
                                                                            theOption, 
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QDoubleValidator(aLineEdt));
        return aLineEdt;
      }
      break;
    case ModelAPI_AttributeTables::INTEGER:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent, 
                                                                            theOption, 
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QIntValidator(aLineEdt));
        return aLineEdt;
      }
      break;
    case ModelAPI_AttributeTables::BOOLEAN: 
      {
        QComboBox* aBox = new QComboBox(theParent);
        aBox->addItem(MYFalse);
        aBox->addItem(MYTrue);
        return aBox;
      }
    }
  }
  return QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
}



//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
CollectionPlugin_WidgetField::
  CollectionPlugin_WidgetField(QWidget* theParent, 
                               ModuleBase_IWorkshop* theWorkshop, 
                               const Config_WidgetAPI* theData):
ModuleBase_WidgetSelector(theParent, theWorkshop, theData), myHeaderEditor(0),
  myIsEditing(false)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);

  // Types definition controls
  QWidget* aTypesWgt = new QWidget(this);
  QFormLayout* aTypesLayout = new QFormLayout(aTypesWgt);
  aTypesLayout->setContentsMargins(0, 0, 0, 0);
  aMainLayout->addWidget(aTypesWgt);

  // Type of shapes
  myShapeTypeCombo = new QComboBox(aTypesWgt);
  QStringList aShapeTypes;
  aShapeTypes << tr("Vertices") << tr("Edges") << tr("Faces") 
    << tr("Solids") << tr("Objects") << tr("Parts");
  myShapeTypeCombo->addItems(aShapeTypes);
  aTypesLayout->addRow(tr("Type of shapes"), myShapeTypeCombo);

  // Type of field
  myFieldTypeCombo = new QComboBox(aTypesWgt);
  QStringList aFieldTypes;
  aFieldTypes << tr("Boolean") << tr("Integer") << tr("Double") 
    << tr("String");
  myFieldTypeCombo->addItems(aFieldTypes);
  myFieldTypeCombo->setCurrentIndex(2);
  aTypesLayout->addRow(tr("Type of field"), myFieldTypeCombo);

  // Number of components
  myNbComponentsSpn = new QSpinBox(aTypesWgt);
  myNbComponentsSpn->setMinimum(1);
  aTypesLayout->addRow(tr("Nb. of components"), myNbComponentsSpn);

  // Steps controls
  QFrame* aStepFrame = new QFrame(this);
  aStepFrame->setFrameShape(QFrame::Box);
  aStepFrame->setFrameStyle(QFrame::StyledPanel);
  QGridLayout* aStepLayout = new QGridLayout(aStepFrame);
  aMainLayout->addWidget(aStepFrame);

  // Current step label
  aStepLayout->addWidget(new QLabel(tr("Current step"), aStepFrame), 0, 0);
  myCurStepLbl = new QLabel("1", aStepFrame);
  QFont aFont = myCurStepLbl->font();
  aFont.setBold(true);
  myCurStepLbl->setFont(aFont);
  aStepLayout->addWidget(myCurStepLbl, 0, 1);

  // Steps slider
  QWidget* aSliderWidget = new QWidget(aStepFrame);
  aStepLayout->addWidget(aSliderWidget, 1, 0, 1, 2);
  QHBoxLayout* aSliderLayout = new QHBoxLayout(aSliderWidget);
  aSliderLayout->setContentsMargins(0, 0, 0, 0);

  aSliderLayout->addWidget(new QLabel("1", aSliderWidget));

  myStepSlider = new QSlider(Qt::Horizontal, aSliderWidget);
  myStepSlider->setTickPosition(QSlider::TicksBelow);
  myStepSlider->setRange(1, 1);
  myStepSlider->setPageStep(myStepSlider->singleStep());
  aSliderLayout->addWidget(myStepSlider, 1);

  myMaxLbl = new QLabel("1", aSliderWidget);
  aSliderLayout->addWidget(myMaxLbl);

  // Stamp value
  myCompNamesList << "Comp 1";
  myStepWgt = new QStackedWidget(aStepFrame);
  aStepLayout->addWidget(myStepWgt, 2, 0, 1, 2);
  appendStepControls();

  // Buttons below
  QWidget* aBtnWgt = new QWidget(this);
  aMainLayout->addWidget(aBtnWgt);
  QHBoxLayout* aBtnLayout = new QHBoxLayout(aBtnWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);

  QPushButton* aAddBtn = new QPushButton(tr("Add step"), aBtnWgt);
  aBtnLayout->addWidget(aAddBtn);

  aBtnLayout->addStretch(1);

  myRemoveBtn = new QPushButton(tr("Remove step"), aBtnWgt);
  aBtnLayout->addWidget(myRemoveBtn);
  myRemoveBtn->setEnabled(false);

  connect(myNbComponentsSpn, SIGNAL(valueChanged(int)), SLOT(onNbCompChanged(int)));
  connect(aAddBtn, SIGNAL(clicked(bool)), SLOT(onAddStep()));
  connect(myRemoveBtn, SIGNAL(clicked(bool)), SLOT(onRemoveStep()));
  connect(myStepSlider, SIGNAL(valueChanged(int)), SLOT(onStepMove(int)));
  connect(myStepSlider, SIGNAL(rangeChanged(int, int)), SLOT(onRangeChanged(int, int)));
  connect(myFieldTypeCombo, SIGNAL(currentIndexChanged(int)), SLOT(onFieldTypeChanged(int)));
  connect(myShapeTypeCombo, SIGNAL(currentIndexChanged(int)), SLOT(onShapeTypeChanged(int)));
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(onFocusChanged(QWidget*, QWidget*)));
}

//**********************************************************************************
void CollectionPlugin_WidgetField::appendStepControls()
{
  QWidget* aWidget = new QWidget(myStepWgt);
  QGridLayout* aStepLayout = new QGridLayout(aWidget);
  aStepLayout->setContentsMargins(0, 0, 0, 0);

  aStepLayout->addWidget(new QLabel(tr("Stamp"), aWidget), 0, 0);

  QSpinBox* aStampSpn = new QSpinBox(aWidget);
  aStepLayout->addWidget(aStampSpn, 0, 1);

  myStampSpnList.append(aStampSpn);

  // Data table
  QTableWidget* aDataTbl = new QTableWidget(1, myCompNamesList.count() + 1, aWidget);
  DataTableItemDelegate* aDelegate = 0;
  if (myDataTblList.isEmpty())
    aDelegate = new DataTableItemDelegate(
      (ModelAPI_AttributeTables::ValueType) myFieldTypeCombo->currentIndex());
  else
    aDelegate = dynamic_cast<DataTableItemDelegate*>(myDataTblList.first()->itemDelegate());

  aDataTbl->setItemDelegate(aDelegate);
  myDataTblList.append(aDataTbl);

  aDataTbl->verticalHeader()->hide();
  aDataTbl->setRowHeight(0, 25);
  aDataTbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  updateHeaders(aDataTbl);

  QTableWidgetItem* aItem = new QTableWidgetItem("Default value");
  aItem->setBackgroundColor(Qt::lightGray);
  aItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  aDataTbl->setItem(0, 0, aItem);

  // Set default value item
  for (int i = 0; i < myCompNamesList.count(); i++) {
    aItem = createDefaultItem();
    aItem->setBackgroundColor(Qt::lightGray);
    aDataTbl->setItem(0, i + 1, aItem);
  }
  aStepLayout->addWidget(aDataTbl, 1, 0, 1, 2);
  connect(aDataTbl, SIGNAL(cellChanged(int, int)), SLOT(onTableEdited(int, int)));

  QAbstractItemDelegate* aDel = aDataTbl->itemDelegate();
  myStepWgt->addWidget(aWidget);
  aDataTbl->horizontalHeader()->viewport()->installEventFilter(this);
}

//**********************************************************************************
void CollectionPlugin_WidgetField::deactivate()
{
  ModuleBase_WidgetSelector::deactivate();
  storeValueCustom();
}


//**********************************************************************************
bool CollectionPlugin_WidgetField::eventFilter(QObject* theObject, QEvent* theEvent)
{
  QObject* aObject = 0;
  foreach(QTableWidget* aTable, myDataTblList) {
    if (aTable->horizontalHeader()->viewport() == theObject) {
      aObject = theObject;
      break;
    }
  }
  if (aObject) {
    if (theEvent->type() == QEvent::MouseButtonDblClick) {
      if (myHeaderEditor) { //delete previous editor
        myHeaderEditor->deleteLater();
        myHeaderEditor = 0;
      }
      QMouseEvent* aMouseEvent = static_cast<QMouseEvent*>(theEvent);
      QHeaderView* aHeader = static_cast<QHeaderView*>(aObject->parent());
      QTableWidget* aTable = static_cast<QTableWidget*>(aHeader->parentWidget());

      int aShift = aTable->horizontalScrollBar()->value();
      int aPos = aMouseEvent->x();
      int aIndex = aHeader->logicalIndex(aHeader->visualIndexAt(aPos));
      if (aIndex > 0) {
        QRect aRect;
        aRect.setLeft(aHeader->sectionPosition(aIndex));
        aRect.setWidth(aHeader->sectionSize(aIndex));
        aRect.setTop(0);
        aRect.setHeight(aHeader->height());
        aRect.adjust(1, 1, -1, -1);
        aRect.translate(-aShift, 0);

        myHeaderEditor = new QLineEdit(aHeader->viewport());
        myHeaderEditor->move(aRect.topLeft());
        myHeaderEditor->resize(aRect.size());
        myHeaderEditor->setFrame(false);
        QString aText = aHeader->model()->
          headerData(aIndex, aHeader->orientation()).toString();
        myHeaderEditor->setText(aText);
        myHeaderEditor->setFocus();
        myEditIndex = aIndex; //save for future use
        myHeaderEditor->installEventFilter(this); //catch focus out event
        //if user presses Enter it should close editor
        connect(myHeaderEditor, SIGNAL(returnPressed()), aTable, SLOT(setFocus()));
        myHeaderEditor->show();
        return true;
      }
    }
  } else if ((theObject == myHeaderEditor) && (theEvent->type() == QEvent::FocusOut)) {
    //QHeaderView* aHeader = 
    //  static_cast<QHeaderView*>(myHeaderEditor->parentWidget()->parentWidget());
    QString aNewTitle = myHeaderEditor->text();
    //save item text
    //aHeader->model()->setHeaderData(myEditIndex, aHeader->orientation(), aNewTitle);
    myCompNamesList.replace(myEditIndex - 1, aNewTitle);
    myHeaderEditor->deleteLater(); //safely delete editor
    myHeaderEditor = 0;
    // Store into data model
    AttributeStringArrayPtr aStringsAttr =
      myFeature->data()->stringArray(CollectionPlugin_Field::COMPONENTS_NAMES_ID());
    aStringsAttr->setValue(myEditIndex - 1, aNewTitle.toStdString());
    foreach(QTableWidget* aTable, myDataTblList) {
      updateHeaders(aTable);
    }
  }
  return ModuleBase_WidgetSelector::eventFilter(theObject, theEvent);
}

//**********************************************************************************
QTableWidgetItem* CollectionPlugin_WidgetField::createDefaultItem() const
{
  QTableWidgetItem* aItem = new QTableWidgetItem();
  switch (myFieldTypeCombo->currentIndex()) {
  case ModelAPI_AttributeTables::DOUBLE:
  case ModelAPI_AttributeTables::INTEGER:
    aItem->setText("0");
    break;
  case ModelAPI_AttributeTables::BOOLEAN: 
    aItem->setText(MYFalse);
    break;
  }
  return aItem;
}

//**********************************************************************************
QTableWidgetItem* CollectionPlugin_WidgetField::
  createValueItem(ModelAPI_AttributeTables::Value& theVal) const
{
  QTableWidgetItem* aItem = new QTableWidgetItem();
  switch (myFieldTypeCombo->currentIndex()) {
  case ModelAPI_AttributeTables::DOUBLE:
    aItem->setText(QString::number(theVal.myDouble));
    break;
  case ModelAPI_AttributeTables::INTEGER:
    aItem->setText(QString::number(theVal.myInt));
    break;
  case ModelAPI_AttributeTables::BOOLEAN: 
    aItem->setText(theVal.myBool? MYTrue : MYFalse);
    break;
  case ModelAPI_AttributeTables::STRING: 
    aItem->setText(theVal.myStr.c_str());
  }
  return aItem;
}

//**********************************************************************************
void CollectionPlugin_WidgetField::updateHeaders(QTableWidget* theDataTbl) const
{
  QStringList aHeaders;
  aHeaders << tr(MYFirstCol);
  aHeaders << myCompNamesList;
  theDataTbl->setHorizontalHeaderLabels(aHeaders);
}

//**********************************************************************************
void CollectionPlugin_WidgetField::removeStepControls()
{
  int aCurWgtId = myStepWgt->currentIndex();
  QWidget* aWgt = myStepWgt->currentWidget();
  myStepWgt->removeWidget(aWgt);

  myStampSpnList.removeAt(aCurWgtId);
  myDataTblList.removeAt(aCurWgtId);
  aWgt->deleteLater();
}

//**********************************************************************************
QList<QWidget*> CollectionPlugin_WidgetField::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  aControls.push_back(myShapeTypeCombo);
  //aControls.push_back(myFieldTypeCombo);
  //aControls.push_back(myNbComponentsSpn);
  return aControls;
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  // Store number of components
  AttributeStringArrayPtr aStringsAttr =
    aData->stringArray(CollectionPlugin_Field::COMPONENTS_NAMES_ID());
  int aNbComps = myCompNamesList.size();
  aStringsAttr->setSize(aNbComps);
  for ( int i = 0; i < aNbComps; i++)
    aStringsAttr->setValue(i, myCompNamesList.at(i).toStdString());

  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  // Store number of steps
  int aNbSteps =  myDataTblList.size();

  // Store Type of the field values
  int aFldType = myFieldTypeCombo->currentIndex();

  AttributeIntArrayPtr aStampsAttr = aData->intArray(CollectionPlugin_Field::STAMPS_ID());
  aStampsAttr->setSize(aNbSteps);
  // Store data
  QTableWidget* aTable = myDataTblList.first();
  int aRows = aTable->rowCount();
  // first column contains selected names which should not be stored
  int aColumns = aTable->columnCount() - 1; 

  aTablesAttr->setSize(aRows, aColumns, aNbSteps);
  aTablesAttr->setType((ModelAPI_AttributeTables::ValueType)aFldType);
  for (int i = 0; i < aNbSteps; i++) {
    aStampsAttr->setValue(i, myStampSpnList.at(i)->value());
    aTable = myDataTblList.at(i);
    for (int j = 0; j < aColumns; j++) {
      for (int k = 0; k < aRows; k++) {
        QString aTblVal = aTable->item(k, j + 1)->text();
        aTablesAttr->setValue(getValue(aTblVal), k, j, i);
      }
    }
  }
  updateObject(myFeature);
  return true;
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::restoreValueCustom()
{
  bool isBlocked;
  DataPtr aData = myFeature->data();

  AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
  if (!aSelList->isInitialized())
    return false;
  std::string aTypeStr = aSelList->selectionType();
  myShapeTypeCombo->setCurrentIndex(getSelectionType(aTypeStr));

  // Get number of components
  AttributeStringArrayPtr aStringsAttr =
  aData->stringArray(CollectionPlugin_Field::COMPONENTS_NAMES_ID());

  myCompNamesList.clear();
  for (int i = 0; i < aStringsAttr->size(); i++) {
    myCompNamesList.append(aStringsAttr->value(i).c_str());
  }
  isBlocked = myNbComponentsSpn->blockSignals(true);
  myNbComponentsSpn->setValue(myCompNamesList.size());
  myNbComponentsSpn->blockSignals(isBlocked);

  AttributeTablesPtr aTablesAttr = aData->tables(CollectionPlugin_Field::VALUES_ID());
  // Get number of steps
  int aNbSteps = aTablesAttr->tables();
  myStepSlider->setMaximum(aNbSteps);
  //myStepSlider->setValue(1);
  // Clear old tables
  myStampSpnList.clear();
  myDataTblList.clear();
  while (myStepWgt->count()) {
    QWidget* aWgt = myStepWgt->widget(myStepWgt->count() - 1);
    myStepWgt->removeWidget(aWgt);
    aWgt->deleteLater();
  }

  while (myStepWgt->count() < aNbSteps)
    appendStepControls();
  //myStepWgt->setCurrentIndex(myStepSlider->value() - 1);

  // Get Type of the field values
  myFieldTypeCombo->setCurrentIndex(aTablesAttr->type());

  AttributeIntArrayPtr aStampsAttr = aData->intArray(CollectionPlugin_Field::STAMPS_ID());
  // Fill data table
  int aRows = aTablesAttr->rows();
  int aCols = aTablesAttr->columns();

  QTableWidgetItem* aItem = 0;
  for (int i = 0; i < aNbSteps; i++) {
    myStampSpnList.at(i)->setValue(aStampsAttr->value(i));
    QTableWidget* aTable = myDataTblList.at(i);
    isBlocked = aTable->blockSignals(true);
    aTable->setRowCount(aRows);
    for (int j = 0; j < aCols + 1; j++) {
      for (int k = 0; k < aRows; k++) {
        if ((j == 0) && (k > 0)) {
          // Add selection names
          AttributeSelectionPtr aAttr = aSelList->value(k - 1);
          aItem = new QTableWidgetItem(aAttr->namingName().c_str());
          aTable->setItem(k, j, aItem);
        } else if (j > 0) {
          // Add Values
          ModelAPI_AttributeTables::Value aVal = aTablesAttr->value(k, j - 1, i);
          aItem = createValueItem(aVal);
          if (k == 0)
            aItem->setBackgroundColor(Qt::lightGray);
          aTable->setItem(k, j, aItem);

        }
      }
    }
    aTable->blockSignals(isBlocked);
  }
  return true;
}

//**********************************************************************************
int CollectionPlugin_WidgetField::getSelectionType(const std::string& theStr) const
{
  if (theStr == "vertex")
    return 0;
  else if (theStr == "edge")
    return 1;
  else if (theStr == "face")
    return 2;
  else if (theStr == "solid")
    return 3;
  else if (theStr == "object")
    return 4;
  else if (theStr == "part")
    return 5;
  return -1;
}


//**********************************************************************************
std::string CollectionPlugin_WidgetField::getSelectionType(int theType) const
{
  switch (theType) {
  case 0: //"Vertices"
    return "vertex";
  case 1: // "Edges"
    return "edge";
  case 2: // "Faces"
    return "face";
  case 3: // "Solids"
    return "solid";
  case 4: // "Results"
    return "object";
  case 5: // "Parts"
    return "part";
  }
  return "";
}

//**********************************************************************************
QIntList CollectionPlugin_WidgetField::shapeTypes() const
{
  QIntList aRes;
  switch (myShapeTypeCombo->currentIndex()) {
  case 0: //"Vertices"
    aRes.append(ModuleBase_Tools::shapeType("vertex"));
    break;
  case 1: // "Edges"
    aRes.append(ModuleBase_Tools::shapeType("edge"));
    break;
  case 2: // "Faces"
    aRes.append(ModuleBase_Tools::shapeType("face"));
    break;
  case 3: // "Solids"
    aRes.append(ModuleBase_Tools::shapeType("solid"));
    break;
  case 4: // "Results"
    aRes.append(ModuleBase_Tools::shapeType("object"));
    break;
  case 5: // "Parts"
    // TODO: Selection mode for Parts
    break;
  }
  return aRes;
}

//**********************************************************************************
ModelAPI_AttributeTables::Value CollectionPlugin_WidgetField::getValue(QString theStrVal) const
{
  ModelAPI_AttributeTables::Value aVal;
  switch (myFieldTypeCombo->currentIndex()) {
  case ModelAPI_AttributeTables::BOOLEAN:
    aVal.myBool = (theStrVal == MYTrue)? true : false;
    break;
  case ModelAPI_AttributeTables::DOUBLE:
    aVal.myDouble = theStrVal.toDouble();
    break;
  case ModelAPI_AttributeTables::INTEGER:
    aVal.myInt = theStrVal.toInt();
    break;
  case ModelAPI_AttributeTables::STRING:
    aVal.myStr = theStrVal.toStdString();
  }
  return aVal;
}


//**********************************************************************************
void CollectionPlugin_WidgetField::onNbCompChanged(int theVal)
{
  int aOldCol = myCompNamesList.count();
  int aNbRows = myDataTblList.first()->rowCount();
  int aDif = theVal - aOldCol;
  QTableWidgetItem* aItem = 0;

  while (myCompNamesList.count() != theVal) {
    if (aDif > 0)
      myCompNamesList.append(QString("Comp %1").arg(myCompNamesList.count() + 1));
    else
      myCompNamesList.removeLast();
  }

  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  aTablesAttr->setSize(aNbRows, myCompNamesList.size(), myDataTblList.size());

  foreach(QTableWidget* aDataTbl, myDataTblList) {
    aDataTbl->setColumnCount(theVal + 1);
    updateHeaders(aDataTbl);
    for (int i = aOldCol; i < myCompNamesList.count(); i++) {
      for (int j = 0; j < aNbRows; j++) {
        aItem = createDefaultItem();
        if (j == 0)
          aItem->setBackgroundColor(Qt::lightGray);
        aDataTbl->setItem(j, i + 1, aItem);
      }
    }
  }
  emit valuesChanged();
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onAddStep()
{
  int aMax = myStepSlider->maximum();
  aMax++;
  myStepSlider->setMaximum(aMax);
  myMaxLbl->setText(QString::number(aMax));
  appendStepControls();
  myStepSlider->setValue(aMax);

  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  aTablesAttr->setSize(aTablesAttr->rows(), aTablesAttr->columns(), myDataTblList.size());


  AttributeSelectionListPtr aSelList = 
    myFeature->data()->selectionList(CollectionPlugin_Field::SELECTED_ID());
  if (!aSelList->isInitialized())
    return;
  int aSelNb = aSelList->size();
  if (aSelNb == 0)
    return;

  int aColumns = myNbComponentsSpn->value() + 1;
  int aRows = aSelNb + 1;
  QTableWidget* aTable = myDataTblList.last();
  aTable->setRowCount(aRows);
  QTableWidgetItem* aItem = 0;
  for(int i = 0; i < aColumns; i++) {
    if (i == 0) {
      for(int j = 1; j < aRows; j++) {
        aItem = new QTableWidgetItem();
        AttributeSelectionPtr aAttr = aSelList->value(j - 1);
        aItem->setText(aAttr->namingName().c_str());
        aItem->setToolTip(aAttr->namingName().c_str());
        aTable->setItem(j, i, aItem);
      }
    } else {
      QString aDefVal = aTable->item(0, i)->text();
      for(int j = 1; j < aRows; j++) {
        aItem = new QTableWidgetItem();
        aItem->setText(aDefVal);
        aTable->setItem(j, i, aItem);
      }
    }
  }
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onRemoveStep()
{
  int aMax = myStepSlider->maximum();
  aMax--;
  myMaxLbl->setText(QString::number(aMax));
  removeStepControls();
  myStepSlider->setMaximum(aMax);

  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  aTablesAttr->setSize(aTablesAttr->rows(), aTablesAttr->columns(), myDataTblList.size());
}

//**********************************************************************************
void CollectionPlugin_WidgetField::clearData()
{
  foreach(QTableWidget* aDataTbl, myDataTblList) {
    aDataTbl->setRowCount(1);
  }
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onStepMove(int theStep)
{
  myCurStepLbl->setText(QString::number(theStep));
  myStepWgt->setCurrentIndex(theStep - 1);
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::
  isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
  return (myShapeTypeCombo->currentIndex() == 5)? false : true;
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onSelectionChanged()
{
  // Ignore selection for Parts mode
  if (myShapeTypeCombo->currentIndex() == 5)
    return;

  QList<ModuleBase_ViewerPrsPtr> aSelected = 
    myWorkshop->selection()->getSelected(ModuleBase_ISelection::AllControls);

  //clearData();
  AttributeSelectionListPtr aSelList = 
    myFeature->data()->selectionList(CollectionPlugin_Field::SELECTED_ID());
  aSelList->clear();

  ResultPtr aResult;
  GeomShapePtr aShape;
  int aNbData = 0;
  foreach(ModuleBase_ViewerPrsPtr aPrs, aSelected) {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
    aShape = aPrs->shape();
    if (!aResult.get() && !aShape.get())
      continue;
    if (!aSelList->isInList(aResult, aShape)) {
      aSelList->append(aResult, aShape);
      aNbData++;
    }
  }
  int aColumns = myDataTblList.first()->columnCount();
  int aRows = myDataTblList.first()->rowCount();
  int aNewRows = aNbData + 1;
  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  aTablesAttr->setSize(aNewRows, aColumns - 1, myDataTblList.size());

  QTableWidgetItem* aItem = 0;
  foreach(QTableWidget* aTable, myDataTblList) {
    aTable->setRowCount(aNewRows);
    if (aNewRows > aRows) { 
      // Add new data
      for(int i = 0; i < aColumns; i++) {
        if (i == 0) {
          for(int j = 1; j < aNewRows; j++) {
            aItem = new QTableWidgetItem();
            AttributeSelectionPtr aAttr = aSelList->value(j - 1);
            aItem->setText(aAttr->namingName().c_str());
            aItem->setToolTip(aAttr->namingName().c_str());
            aTable->setItem(j, i, aItem);
          }
        } else {
          QString aDefVal = aTable->item(0, i)->text();
          for(int j = aRows; j < aNewRows; j++) {
            aItem = new QTableWidgetItem();
            aItem->setText(aDefVal);
            aTable->setItem(j, i, aItem);
          }
        }
      }
    } else {
      // Update only selection name
      for(int j = 1; j < aNewRows - 1; j++) {
        AttributeSelectionPtr aAttr = aSelList->value(j);
        aTable->item(j, 0)->setText(aAttr->namingName().c_str());
        aTable->item(j, 0)->setToolTip(aAttr->namingName().c_str());
      }
    }
  }
  emit valuesChanged();
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onFieldTypeChanged(int theIdx)
{
  DataTableItemDelegate* aDelegate = 0;
  aDelegate = dynamic_cast<DataTableItemDelegate*>(myDataTblList.first()->itemDelegate());
  if (aDelegate) {
    ModelAPI_AttributeTables::ValueType aOldType = aDelegate->dataType();
    if (aOldType != theIdx) {
      aDelegate->setDataType((ModelAPI_AttributeTables::ValueType)theIdx);
      int aColumns = myDataTblList.first()->columnCount();
      int aRows = myDataTblList.first()->rowCount();
      foreach(QTableWidget* aTable, myDataTblList) {
        for(int i = 1; i < aColumns; i++) {
          for(int j = 0; j < aRows; j++) {
            switch (theIdx) {
            case ModelAPI_AttributeTables::DOUBLE:
            case ModelAPI_AttributeTables::INTEGER:
              if ((aOldType == ModelAPI_AttributeTables::BOOLEAN) ||
                  (aOldType == ModelAPI_AttributeTables::STRING)) {
                    aTable->item(j, i)->setText("0");
              }
              break;
            case ModelAPI_AttributeTables::BOOLEAN: 
              aTable->item(j, i)->setText(MYFalse);
              break;
            }
          }
        }
      }
      emit valuesChanged();
    }
  }
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onTableEdited(int theRow, int theCol)
{
  // Do not store here column of names
  if (theCol == 0)
    return;
  if (!myFeature.get())
    return;
  QTableWidget* aTable = static_cast<QTableWidget*>(sender());
  int aNb = myDataTblList.indexOf(aTable);
  if (aNb == -1)
    return;
  ModelAPI_AttributeTables::Value aVal = getValue(aTable->item(theRow, theCol)->text());

  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  if (aTablesAttr->isInitialized())
    aTablesAttr->setValue(aVal,theRow, theCol - 1, aNb);
  else
    emit valuesChanged();
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onShapeTypeChanged(int theType)
{
  activateSelectionAndFilters(theType == 5? false:true);

  AttributeSelectionListPtr aSelList = 
    myFeature->data()->selectionList(CollectionPlugin_Field::SELECTED_ID());

  std::string aTypeName = getSelectionType(theType);
  if (aTypeName == aSelList->selectionType())
    return;
  aSelList->setSelectionType(aTypeName);

  //Clear old selection
  clearData();
  aSelList->clear();
  AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  aTablesAttr->setSize(1, myNbComponentsSpn->value(), myDataTblList.size());
  emit valuesChanged();
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::processEnter()
{
  if (myIsEditing) {
    myIsEditing = false;
    return true;
  }
  return false;
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onFocusChanged(QWidget* theOld, QWidget* theNew)
{
  if (theNew && (!myIsEditing))
    myIsEditing = dynamic_cast<QLineEdit*>(theNew);
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onRangeChanged(int theMin, int theMax)
{
  myMaxLbl->setText(QString::number(theMax));
  myRemoveBtn->setEnabled(theMax > 1);
}


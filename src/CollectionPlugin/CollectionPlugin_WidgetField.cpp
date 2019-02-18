// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "CollectionPlugin_WidgetField.h"
#include "CollectionPlugin_Field.h"

#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IPropertyPanel.h>

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
#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>

const char* MYFirstCol = "Shape";
const char* MYTrue = "True";
const char* MYFalse = "False";

DataTableItemDelegate::DataTableItemDelegate(ModelAPI_AttributeTables::ValueType theType)
  : QStyledItemDelegate(), myType(theType)
{
}


QWidget* DataTableItemDelegate::createEditor(QWidget* theParent,
                                             const QStyleOptionViewItem & theOption,
                                             const QModelIndex& theIndex ) const
{
  QWidget* aEditor = 0;
  if ((theIndex.column() == 0) && (theIndex.row() > 0)) {
    QWidget* aWgt = QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
    QLineEdit* aEdt = static_cast<QLineEdit*>(aWgt);
    aEdt->setReadOnly(true);
    aEditor = aEdt;
  } else {
    QLineEdit* aLineEdt = 0;
    switch (myType) {
    case ModelAPI_AttributeTables::DOUBLE:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent,
                                                                            theOption,
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QDoubleValidator(aLineEdt));
        aEditor = aLineEdt;
      }
      break;
    case ModelAPI_AttributeTables::INTEGER:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent,
                                                                            theOption,
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QIntValidator(aLineEdt));
        aEditor = aLineEdt;
      }
      break;
    case ModelAPI_AttributeTables::BOOLEAN:
      {
        QComboBox* aBox = new QComboBox(theParent);
        aBox->addItem(MYFalse);
        aBox->addItem(MYTrue);
        aEditor = aBox;
      }
      break;
    default:
      aEditor = QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
    }
  }
  if (myType == ModelAPI_AttributeTables::BOOLEAN)
    connect(aEditor, SIGNAL(currentTextChanged(const QString&)),
      SLOT(onEditItem(const QString&)));
  else
    connect(aEditor, SIGNAL(textEdited(const QString&)),
      SLOT(onEditItem(const QString&)));
  return aEditor;
}

void DataTableItemDelegate::onEditItem(const QString& theText)
{
  QWidget* aWgt = dynamic_cast<QWidget*>(sender());
  commitData(aWgt);
}



//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
CollectionPlugin_WidgetField::
  CollectionPlugin_WidgetField(QWidget* theParent,
                               ModuleBase_IWorkshop* theWorkshop,
                               const Config_WidgetAPI* theData):
ModuleBase_WidgetSelector(theParent, theWorkshop, theData), myHeaderEditor(0),
  myIsTabEdit(false), myActivation(false)
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

  myDelegate =
    new DataTableItemDelegate((ModelAPI_AttributeTables::ValueType)
    myFieldTypeCombo->currentIndex());

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
  aDataTbl->installEventFilter(this);
  aDataTbl->setItemDelegate(myDelegate);

  QIntList aColWidth;
  if (!myDataTblList.isEmpty()) {
    QTableWidget* aFirstTable = myDataTblList.first();
    for (int i = 0; i < aFirstTable->columnCount(); i++)
      aColWidth.append(aFirstTable->columnWidth(i));
  }
  myDataTblList.append(aDataTbl);

  aDataTbl->verticalHeader()->hide();
  aDataTbl->setRowHeight(0, 25);
  aDataTbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  connect(aDataTbl->horizontalHeader(), SIGNAL(sectionResized(int, int, int)),
          SLOT(onColumnResize(int, int, int)));

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

  if (aColWidth.length() > 0) {
    for (int i = 0; i < aDataTbl->columnCount(); i++) {
      if (i < aColWidth.size())
      aDataTbl->setColumnWidth(i, aColWidth.at(i));
    }
  }
  aStepLayout->addWidget(aDataTbl, 1, 0, 1, 2);
  connect(aDataTbl, SIGNAL(cellChanged(int, int)), SLOT(onTableEdited(int, int)));

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
//void CollectionPlugin_WidgetField::showEvent(QShowEvent* theEvent)
//{
//  myShapeTypeCombo->setEnabled(!isEditingMode());
//  myFieldTypeCombo->setEnabled(!isEditingMode());
//  myNbComponentsSpn->setEnabled(!isEditingMode());
//}

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
    QString aNewTitle = myHeaderEditor->text();
    //save item text
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
  } else if (theEvent->type() == QEvent::FocusIn) {
    QTableWidget* aTable = dynamic_cast<QTableWidget*>(theObject);
    if (aTable) {
      ModuleBase_IPropertyPanel* aPanel = myWorkshop->propertyPanel();
      if (aPanel->activeWidget() != this) {
        myActivation = true;
        aPanel->activateWidget(this, false);
      }
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
  case ModelAPI_AttributeTables::STRING:
    aItem->setText("");
    break;
  }
  return aItem;
}

//**********************************************************************************
QTableWidgetItem* CollectionPlugin_WidgetField::
  createValueItem(ModelAPI_AttributeTables::Value& theVal) const
{
  QTableWidgetItem* aItem = new QTableWidgetItem();
  aItem->setText(getValueText(theVal));
  return aItem;
}

//**********************************************************************************
QString CollectionPlugin_WidgetField::getValueText(ModelAPI_AttributeTables::Value& theVal) const
{
  switch (myFieldTypeCombo->currentIndex()) {
  case ModelAPI_AttributeTables::DOUBLE:
    return QString::number(theVal.myDouble);
  case ModelAPI_AttributeTables::INTEGER:
    return QString::number(theVal.myInt);
  case ModelAPI_AttributeTables::BOOLEAN:
    return theVal.myBool? MYTrue : MYFalse;
  case ModelAPI_AttributeTables::STRING:
    return theVal.myStr.c_str();
  }
  return "";
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
  aControls.append(myShapeTypeCombo);
  aControls.append(myFieldTypeCombo);
  aControls.append(myNbComponentsSpn);
  if (myStampSpnList.size() > 0)
    aControls.append(myStampSpnList.first());
  if (myDataTblList.size() > 0)
    aControls.append(myDataTblList.first());

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
  return true;
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::restoreValueCustom()
{
  bool isBlocked;
  DataPtr aData = myFeature->data();

  AttributeSelectionListPtr aSelList = aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
  std::string aTypeStr = aSelList->selectionType();
  if (aTypeStr == "")
    return false; // The attribute is not initialized
  isBlocked = myShapeTypeCombo->blockSignals(true);
  myShapeTypeCombo->setCurrentIndex(getSelectionType(aTypeStr));
  myShapeTypeCombo->blockSignals(isBlocked);

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
  while (myDataTblList.count() > aNbSteps) {
    QWidget* aWgt = myStepWgt->widget(myStepWgt->count() - 1);
    myStepWgt->removeWidget(aWgt);
    aWgt->deleteLater();

    myStampSpnList.removeLast();
    myDataTblList.removeLast();
  }
  while (myDataTblList.count() < aNbSteps)
    appendStepControls();
  //myStepWgt->setCurrentIndex(myStepSlider->value() - 1);
  clearData();

  // Get Type of the field values
  isBlocked = myFieldTypeCombo->blockSignals(true);
  myFieldTypeCombo->setCurrentIndex(aTablesAttr->type());
  myFieldTypeCombo->blockSignals(isBlocked);
  myDelegate->setDataType(aTablesAttr->type());

  AttributeIntArrayPtr aStampsAttr = aData->intArray(CollectionPlugin_Field::STAMPS_ID());
  // Fill data table
  int aRows = aTablesAttr->rows();
  int aCols = aTablesAttr->columns();

  // Get width of columns
  QIntList aColWidth;
  QTableWidget* aFirstTable = myDataTblList.first();
  for (int i = 0; i < aFirstTable->columnCount(); i++)
    aColWidth.append(aFirstTable->columnWidth(i));

  QTableWidgetItem* aItem = 0;
  for (int i = 0; i < aNbSteps; i++) {
    myStampSpnList.at(i)->setValue(aStampsAttr->value(i));
    QTableWidget* aTable = myDataTblList.at(i);
    isBlocked = aTable->blockSignals(true);
    aTable->setRowCount(aRows);
    aTable->setColumnCount(aCols + 1);
    updateHeaders(aTable);
    for (int j = 0; j < aCols + 1; j++) {
      for (int k = 0; k < aRows; k++) {
        aItem = aTable->item(k, j);
        if ((j == 0) && (k > 0)) {
          // Add selection names
          AttributeSelectionPtr aAttr = aSelList->value(k - 1);
          if (aItem) {
            aItem->setText(aAttr->namingName().c_str());
          } else {
            aItem = new QTableWidgetItem(aAttr->namingName().c_str());
            aTable->setItem(k, j, aItem);
          }
        } else if (j > 0) {
          // Add Values
          ModelAPI_AttributeTables::Value aVal = aTablesAttr->value(k, j - 1, i);
          if (aItem) {
            aItem->setText(getValueText(aVal));
          } else {
            aItem = createValueItem(aVal);
            if (k == 0)
              aItem->setBackgroundColor(Qt::lightGray);
            aTable->setItem(k, j, aItem);
          }
        }
      }
    }
    // Restore columns width
    for (int i = 0; i < aTable->columnCount(); i++) {
      if (i < aColWidth.size())
        aTable->setColumnWidth(i, aColWidth.at(i));
    }

    aTable->blockSignals(isBlocked);
  }
  return true;
}

//**********************************************************************************
int CollectionPlugin_WidgetField::getSelectionType(const std::string& theStr) const
{
  QString aType(theStr.c_str());
  aType = aType.toLower();
  if (aType == "vertex")
    return 0;
  else if (aType == "edge")
    return 1;
  else if (aType == "face")
    return 2;
  else if (aType == "solid")
    return 3;
  else if (aType == "object")
    return 4;
  else if (aType == "part")
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
        aItem = aDataTbl->item(j, i + 1);
        if (!aItem) {
          aItem = createDefaultItem();
          if (j == 0)
            aItem->setBackgroundColor(Qt::lightGray);
          aDataTbl->setItem(j, i + 1, aItem);
        }
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
        aItem = aTable->item(j, i);
        if (!aItem) {
          aItem = new QTableWidgetItem();
          aTable->setItem(j, i, aItem);
        }
        AttributeSelectionPtr aAttr = aSelList->value(j - 1);
        aItem->setText(aAttr->namingName().c_str());
        aItem->setToolTip(aAttr->namingName().c_str());
      }
    } else {
      QString aDefVal = aTable->item(0, i)->text();
      for(int j = 1; j < aRows; j++) {
        aItem = aTable->item(j, i);
        if (!aItem) {
          aItem = new QTableWidgetItem();
          aTable->setItem(j, i, aItem);
        }
        aItem->setText(aDefVal);
      }
    }
  }
  emit valuesChanged();
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onRemoveStep()
{
  int aMax = myStepSlider->maximum();
  aMax--;
  myMaxLbl->setText(QString::number(aMax));
  removeStepControls();
  myStepSlider->setMaximum(aMax);

  //AttributeTablesPtr aTablesAttr = myFeature->data()->tables(CollectionPlugin_Field::VALUES_ID());
  //aTablesAttr->setSize(aTablesAttr->rows(), aTablesAttr->columns(), myDataTblList.size());
  emit valuesChanged();
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
  isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
  return (myShapeTypeCombo->currentIndex() == 5)? false : true;
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::
  setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues, const bool theToValidate)
{
  if (myActivation) {
    myActivation = false;
    return false;
  }
  // Ignore selection for Parts mode
  if (myShapeTypeCombo->currentIndex() == 5)
    return false;

  QList<ModuleBase_ViewerPrsPtr> aSelected;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt;
  for (anIt = theValues.begin(); anIt != theValues.end(); anIt++) {
    ModuleBase_ViewerPrsPtr aValue = *anIt;
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aValue->object());
    if (theToValidate && aRes.get()) {
      if (myShapeTypeCombo->currentIndex() > 3)
        aSelected.append(aValue);
      else if (acceptSubShape(aValue->shape(), aRes))
        aSelected.append(aValue);
    }
  }
  AttributeSelectionListPtr aSelList =
    myFeature->data()->selectionList(CollectionPlugin_Field::SELECTED_ID());
  aSelList->setSelectionType(getSelectionType(myShapeTypeCombo->currentIndex()));
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
            aItem = aTable->item(j, i);
            if (!aItem) {
              aItem = new QTableWidgetItem();
              aTable->setItem(j, i, aItem);
            }
            AttributeSelectionPtr aAttr = aSelList->value(j - 1);
            aItem->setText(aAttr->namingName().c_str());
            aItem->setToolTip(aAttr->namingName().c_str());
          }
        } else {
          QString aDefVal = aTable->item(0, i)->text();
          for(int j = aRows; j < aNewRows; j++) {
            aItem = aTable->item(j, i);
            if (!aItem) {
              aItem = new QTableWidgetItem();
              aTable->setItem(j, i, aItem);
            }
            aItem->setText(aDefVal);
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
  return true;
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onFieldTypeChanged(int theIdx)
{
  ModelAPI_AttributeTables::ValueType aOldType = myDelegate->dataType();
  if (aOldType != theIdx) {
    myDelegate->setDataType((ModelAPI_AttributeTables::ValueType)theIdx);
    int aColumns = myDataTblList.first()->columnCount();
    int aRows = myDataTblList.first()->rowCount();
    foreach(QTableWidget* aTable, myDataTblList) {
      for(int i = 1; i < aColumns; i++) {
        for(int j = 0; j < aRows; j++) {
          switch (theIdx) {
          case ModelAPI_AttributeTables::DOUBLE:
          case ModelAPI_AttributeTables::INTEGER:
            aTable->item(j, i)->setText("0");
            break;
          case ModelAPI_AttributeTables::BOOLEAN:
            aTable->item(j, i)->setText(MYFalse);
            break;
          case ModelAPI_AttributeTables::STRING:
            aTable->item(j, i)->setText("");
            break;
          }
        }
      }
    }
    emit valuesChanged();
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
  updateSelectionModesAndFilters(theType == 5? false:true);

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
  updateObject(myFeature);
  emit valuesChanged();
}

//**********************************************************************************
bool CollectionPlugin_WidgetField::processEnter()
{
  if (myIsTabEdit) {
    myIsTabEdit = false;
    return true;
  }
  QWidget* aCurrWgt = qApp->focusWidget();
  int aCurWgtId = myStepWgt->currentIndex();
  if ((aCurrWgt == myShapeTypeCombo) ||
      (aCurrWgt == myFieldTypeCombo) ||
      (aCurrWgt == myNbComponentsSpn) ||
      (aCurrWgt == myStampSpnList[aCurWgtId]) ||
      (aCurrWgt == myDataTblList[aCurWgtId])) {
    setFocus();
    return true;
  }
  return false;
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onFocusChanged(QWidget* theOld, QWidget* theNew)
{
  if (theNew && (!myIsTabEdit))
    myIsTabEdit = dynamic_cast<QLineEdit*>(theNew);
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onRangeChanged(int theMin, int theMax)
{
  myMaxLbl->setText(QString::number(theMax));
  myRemoveBtn->setEnabled(theMax > 1);
}

//**********************************************************************************
void CollectionPlugin_WidgetField::onColumnResize(int theIndex, int theOld, int theNew)
{
  if (myDataTblList.count() < 2)
    return;
  QObject* aSender = sender();
  foreach(QTableWidget* aTable, myDataTblList) {
    if (aTable->horizontalHeader() != aSender)
      aTable->setColumnWidth(theIndex, theNew);
  }
}

//**********************************************************************************
QList<std::shared_ptr<ModuleBase_ViewerPrs>>
  CollectionPlugin_WidgetField::getAttributeSelection() const
{
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aList;
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributeSelectionListPtr aSelList =
      aData->selectionList(CollectionPlugin_Field::SELECTED_ID());
    AttributeSelectionPtr aAttr;
    ObjectPtr anObject;
    for (int i = 0; i < aSelList->size(); i++) {
      aAttr = aSelList->value(i);
      ModuleBase_ViewerPrsPtr
        aPrs(new ModuleBase_ViewerPrs(aAttr->context(), aAttr->value(), NULL));
      aList.append(aPrs);
    }
  }
  return aList;
}
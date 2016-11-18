// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetField.cpp
// Created:     16 Nov 2016
// Author:      Vitaly SMETANNIKOV

#include "CollectionPlugin_WidgetField.h"

#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>

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


class DataTableItemDelegate : public QStyledItemDelegate
{
public:
  enum DataType {
    DoubleType,
    IntegerType,
    BooleanType,
    StringType };

  DataTableItemDelegate(DataType theType) : QStyledItemDelegate() { myType = theType; }

  virtual QWidget* createEditor(QWidget* theParent, 
                                const QStyleOptionViewItem & theOption, 
                                const QModelIndex& theIndex) const;

  virtual void setModelData(QWidget* theEditor, QAbstractItemModel* theModel, 
                            const QModelIndex& theIndex) const;

  DataType dataType() const { return myType; }

  void setDataType(DataType theType) { myType = theType; }

private:
  DataType myType;
};

QWidget* DataTableItemDelegate::createEditor(QWidget* theParent, 
                                             const QStyleOptionViewItem & theOption, 
                                             const QModelIndex& theIndex ) const
{
  if ((theIndex.column() != 0) && (theIndex.row() != 0)) {
    QLineEdit* aLineEdt = 0;
    switch (myType) {
    case DoubleType:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent, 
                                                                            theOption, 
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QDoubleValidator(aLineEdt));
        return aLineEdt;
      }
    case IntegerType:
      aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent, 
                                                                            theOption, 
                                                                            theIndex));
      if (aLineEdt) {
        aLineEdt->setValidator(new QIntValidator(aLineEdt));
        return aLineEdt;
      }
    case BooleanType: 
      {
        QComboBox* aBox = new QComboBox(theParent);
        aBox->addItem("True");
        aBox->addItem("False");
        return aBox;
      }
    }
  }
  return QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
}


void DataTableItemDelegate::setModelData(QWidget* theEditor, QAbstractItemModel* theModel, 
                                         const QModelIndex& theIndex) const
{
  QComboBox* aBox = dynamic_cast<QComboBox*>(theEditor);
  if (aBox) {
    theModel->setData(theIndex, aBox->currentText());
  } else 
    QStyledItemDelegate::setModelData(theEditor, theModel, theIndex);
}



CollectionPlugin_WidgetField::
  CollectionPlugin_WidgetField(QWidget* theParent, 
                               ModuleBase_IWorkshop* theWorkshop, 
                               const Config_WidgetAPI* theData):
ModuleBase_WidgetSelector(theParent, theWorkshop, theData)
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
    << tr("Solids") << tr("Results") << tr("Parts");
  myShapeTypeCombo->addItems(aShapeTypes);
  aTypesLayout->addRow(tr("Type of shapes"), myShapeTypeCombo);

  // Type of field
  myFieldTypeCombo = new QComboBox(aTypesWgt);
  QStringList aFieldTypes;
  aFieldTypes << tr("Double") << tr("Integer") << tr("Boolean") 
    << tr("String");
  myFieldTypeCombo->addItems(aFieldTypes);
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
  connect(myFieldTypeCombo, SIGNAL(currentIndexChanged(int)), SLOT(onFieldTypeChanged(int)));
}

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
  QTableWidget* aDataTbl = new QTableWidget(2, myCompNamesList.count() + 1, aWidget);
  aDataTbl->setItemDelegate(
    new DataTableItemDelegate((DataTableItemDelegate::DataType) myFieldTypeCombo->currentIndex()));
  aDataTbl->verticalHeader()->hide();
  aDataTbl->horizontalHeader()->hide();
  aDataTbl->setRowHeight(0, 25);
  aDataTbl->setRowHeight(1, 25);

  QTableWidgetItem* aItem = new QTableWidgetItem("Shape");
  aItem->setBackgroundColor(Qt::lightGray);
  aItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  aDataTbl->setItem(0, 0, aItem);

  aItem = new QTableWidgetItem("Default value");
  aItem->setBackgroundColor(Qt::lightGray);
  aItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  aDataTbl->setItem(1, 0, aItem);

  for (int i = 0; i < myCompNamesList.count(); i++) {
    aItem = new QTableWidgetItem(myCompNamesList[i]);
    aItem->setBackgroundColor(Qt::lightGray);
    aDataTbl->setItem(0, i + 1, aItem);

    aItem = new QTableWidgetItem("0");
    aItem->setBackgroundColor(Qt::lightGray);
    aDataTbl->setItem(1, i + 1, aItem);
  }
  aStepLayout->addWidget(aDataTbl, 1, 0, 1, 2);

  QAbstractItemDelegate* aDel = aDataTbl->itemDelegate();
  myDataTblList.append(aDataTbl);

  myStepWgt->addWidget(aWidget);
}

void CollectionPlugin_WidgetField::removeStepControls()
{
  int aCurWgtId = myStepWgt->currentIndex();
  myStepWgt->removeWidget(myStepWgt->currentWidget());

  myStampSpnList.removeAt(aCurWgtId);
  myDataTblList.removeAt(aCurWgtId);
}


QList<QWidget*> CollectionPlugin_WidgetField::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  //aControls.push_back(myShapeTypeCombo);
  //aControls.push_back(myFieldTypeCombo);
  //aControls.push_back(myNbComponentsSpn);
  return aControls;
}

bool CollectionPlugin_WidgetField::storeValueCustom()
{
  //AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  //AttributeIntegerPtr aValueAttribute =
  //                      std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  //aValueAttribute->setValue(myComboBox->currentIndex());
  //updateObject(myFeature);
  return true;
}

bool CollectionPlugin_WidgetField::restoreValueCustom()
{
  //AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  //AttributeIntegerPtr aValueAttribute =
  //                      std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);

  //bool isBlocked = myComboBox->blockSignals(true);
  //myComboBox->setCurrentIndex(aValueAttribute->value());
  //myComboBox->blockSignals(isBlocked);

  return true;
}

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

  foreach(QTableWidget* aDataTbl, myDataTblList) {
    aDataTbl->setColumnCount(theVal + 1);
    for (int i = 0; i < myCompNamesList.count(); i++) {
      for (int j = 0; j < aNbRows; j++) {
        aItem = new QTableWidgetItem();
        if (j == 0)
          aItem->setText(myCompNamesList.at(i));
        else
          aItem->setText("0");
        if (j < 3)
          aItem->setBackgroundColor(Qt::lightGray);
        aDataTbl->setItem(j, i + aOldCol + 1, aItem);
      }
    }
  }
}

void CollectionPlugin_WidgetField::onAddStep()
{
  int aMax = myStepSlider->maximum();
  aMax++;
  myStepSlider->setMaximum(aMax);
  myMaxLbl->setText(QString::number(aMax));
  appendStepControls();
  myStepSlider->setValue(aMax);
  myRemoveBtn->setEnabled(aMax > 1);
}

void CollectionPlugin_WidgetField::onRemoveStep()
{
  int aMax = myStepSlider->maximum();
  aMax--;
  myMaxLbl->setText(QString::number(aMax));
  removeStepControls();
  myStepSlider->setMaximum(aMax);
  myRemoveBtn->setEnabled(aMax > 1);
}

void CollectionPlugin_WidgetField::clearData()
{
  int aNbRows = myDataTblList.first()->rowCount();
  int aNbCol = myDataTblList.first()->columnCount();

  foreach(QTableWidget* aDataTbl, myDataTblList) {
    QString aDefValue;
    for (int i = 1; i < aNbCol; i++) {
      aDefValue = aDataTbl->item(1, i)->text();
      for (int j = 2; j < aNbRows; j++) {
        aDataTbl->item(j, i)->setText(aDefValue);
      }
    }
  }
}

void CollectionPlugin_WidgetField::onStepMove(int theStep)
{
  myCurStepLbl->setText(QString::number(theStep));
  myStepWgt->setCurrentIndex(theStep - 1);
}

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

bool CollectionPlugin_WidgetField::
  isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
  return true;
}

void CollectionPlugin_WidgetField::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = 
    myWorkshop->selection()->getSelected(ModuleBase_ISelection::AllControls);

  clearData();

  foreach(ModuleBase_ViewerPrsPtr aPrs, aSelected) {

  }
}

void CollectionPlugin_WidgetField::onFieldTypeChanged(int theIdx)
{
  DataTableItemDelegate* aDelegate = 0;
  foreach(QTableWidget* aTable, myDataTblList) {
    aDelegate = dynamic_cast<DataTableItemDelegate*>(aTable->itemDelegate());
    if (aDelegate)
      aDelegate->setDataType((DataTableItemDelegate::DataType)theIdx);
  }
}

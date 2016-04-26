// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_WidgetParamsMgr.cpp
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#include "ParametersPlugin_WidgetParamsMgr.h"
#include "ParametersPlugin_Parameter.h"
#include "ParametersPlugin_Validators.h"

#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>

#include <ModuleBase_Tools.h>

#include <Events_Loop.h>

#include <QLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMessageBox>
#include <QTimer>

enum ColumnType {
  Col_Name,
  Col_Equation,
  Col_Result,
  Col_Comment
};

const char* NoName = "<NoName>";
const char* NoValue = "<NoValue>";

class ParametersPlugin_ItemDelegate : public QStyledItemDelegate
{
public:
  ParametersPlugin_ItemDelegate(QObject* thaParent) : 
      QStyledItemDelegate(thaParent) {}

  virtual void paint(QPainter* painter, 
    const QStyleOptionViewItem& option, 
    const QModelIndex& index ) const;
  
  virtual QWidget* createEditor(QWidget* parent, 
                                const QStyleOptionViewItem& option, 
                                const QModelIndex& index) const;

  bool isEditable(const QModelIndex& theIndex) const;

  QModelIndex editIndex() const { return myEditingIdx; }

private:
  mutable QModelIndex myEditingIdx;
};

bool ParametersPlugin_ItemDelegate::isEditable(const QModelIndex& theIndex) const
{
  QModelIndex aParent = theIndex.parent();
  if (aParent.isValid() && (aParent.row() == 0)) {
    if (theIndex.column() == 2)
      return false;
  } else
    return false;
  return true;
}

void ParametersPlugin_ItemDelegate::paint(QPainter* painter, 
                                          const QStyleOptionViewItem& option, 
                                          const QModelIndex& index ) const
{
  QBrush aBrush = painter->brush();
  QPen aPen = painter->pen();
  if (!isEditable(index))
    painter->setBrush(Qt::lightGray);

  painter->setPen(Qt::darkGray);
  painter->drawRect(option.rect);
  painter->setPen(aPen);

  QStyledItemDelegate::paint(painter, option, index);
  painter->setBrush(aBrush);
}

QWidget* ParametersPlugin_ItemDelegate::createEditor(QWidget* parent, 
                                                     const QStyleOptionViewItem& option, 
                                                     const QModelIndex& index) const
{
  myEditingIdx = index;
  return QStyledItemDelegate::createEditor(parent, option, index);
}


/////////////////////////////////////////////////////////////////////////////////////////////////

ParametersPlugin_WidgetParamsMgr::ParametersPlugin_WidgetParamsMgr(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);

  myTable = new QTreeWidget(this);
  myTable->setColumnCount(4);
  QStringList aHeaders;
  aHeaders << tr("Name") << tr("Equation") << tr("Result") << tr("Comment");
  myTable->setHeaderLabels(aHeaders);
  myTable->setColumnWidth(Col_Name, 200);
  myTable->setColumnWidth(Col_Equation, 100);
  myTable->setColumnWidth(Col_Result, 80);
  myTable->setColumnWidth(Col_Comment, 200);
  myTable->setMinimumWidth(600);
  myTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  myTable->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(myTable, SIGNAL(doubleClicked(const QModelIndex&)),
          SLOT(onDoubleClick(const QModelIndex&)));
  connect(myTable, SIGNAL(itemSelectionChanged()), SLOT(onSelectionChanged()));

  myDelegate = new ParametersPlugin_ItemDelegate(myTable);
  connect(myDelegate, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), 
          SLOT(onCloseEditor(QWidget*, QAbstractItemDelegate::EndEditHint)));

  myTable->setItemDelegate(myDelegate);
  aLayout->addWidget(myTable);

  // Define root nodes
  QStringList aNames;
  aNames<<tr("Parameters");
  myParameters = new QTreeWidgetItem(aNames);
  myTable->addTopLevelItem(myParameters);

  aNames.clear();
  aNames<<tr("Features");
  myFeatures = new QTreeWidgetItem(aNames);
  myTable->addTopLevelItem(myFeatures);

  QHBoxLayout* aBtnLayout = new QHBoxLayout(this);

  myUpBtn = new QToolButton(this);
  myUpBtn->setArrowType(Qt::UpArrow);
  connect(myUpBtn, SIGNAL(clicked(bool)), SLOT(onUp()));
  aBtnLayout->addWidget(myUpBtn);

  myDownBtn = new QToolButton(this);
  myDownBtn->setArrowType(Qt::DownArrow);
  connect(myDownBtn, SIGNAL(clicked(bool)), SLOT(onDown()));
  aBtnLayout->addWidget(myDownBtn);

  aBtnLayout->addStretch();

  myAddBtn = new QPushButton(tr("Add"), this);
  connect(myAddBtn, SIGNAL(clicked(bool)), SLOT(onAdd()));
  aBtnLayout->addWidget(myAddBtn);

  myInsertBtn = new QPushButton(tr("Insert"), this);
  connect(myInsertBtn, SIGNAL(clicked(bool)), SLOT(onInsert()));
  aBtnLayout->addWidget(myInsertBtn);

  myRemoveBtn = new QPushButton(tr("Remove"), this);
  connect(myRemoveBtn, SIGNAL(clicked(bool)), SLOT(onRemove()));
  aBtnLayout->addWidget(myRemoveBtn);

  aLayout->addLayout(aBtnLayout);

  onSelectionChanged();
}

QList<QWidget*> ParametersPlugin_WidgetParamsMgr::getControls() const
{
  QList<QWidget*> aList;

  return aList;
}

void ParametersPlugin_WidgetParamsMgr::selectItemScroll(QTreeWidgetItem* aItem)
{
  myTable->clearSelection();
  QModelIndex aParent = myTable->model()->index(0, 0);
  int aChildIdx = myParameters->indexOfChild(aItem);
  QModelIndex aIndex = myTable->model()->index(aChildIdx, Col_Name, aParent);
  myTable->selectionModel()->select(aIndex, 
    QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
  myTable->scrollToItem(aItem);
}


bool ParametersPlugin_WidgetParamsMgr::storeValueCustom()
{
  ParametersPlugin_ExpressionValidator aValidator;
  std::list<std::string> aArgs;
  std::string aAttrId = ParametersPlugin_Parameter::VARIABLE_ID();
  std::string aErr;
  int aId = 0;
  foreach(FeaturePtr aFeature, myParametersList) {
    if (!aValidator.isValid(aFeature->attribute(aAttrId), aArgs, aErr)) {
      QMessageBox::warning(this, tr("Warning"), aErr.c_str());
      selectItemScroll(myParameters->child(aId));
      return false;
    }
    aId++;
  }
  return true;
}

bool ParametersPlugin_WidgetParamsMgr::restoreValueCustom()
{
  return true;
}

void ParametersPlugin_WidgetParamsMgr::activateCustom()
{
  updateParametersFeatures();
  updateParametersPart();
  updateFeaturesPart();

  myFeatures->setExpanded(true);
  myParameters->setExpanded(true);
}

void ParametersPlugin_WidgetParamsMgr::updateParametersFeatures()
{
  myParametersList.clear();
  FeaturePtr aFeature = feature();
  DocumentPtr aDoc = aFeature->document();
  int aNbParam = aDoc->size(ModelAPI_ResultParameter::group());
  ObjectPtr aObj;
  FeaturePtr aParamFeature;
  for (int i = 0; i < aNbParam; i++) {
    aObj = aDoc->object(ModelAPI_ResultParameter::group(), i);
    aParamFeature = ModelAPI_Feature::feature(aObj);
    if (aParamFeature.get() && (aParamFeature->getKind() == ParametersPlugin_Parameter::ID())) {
      myParametersList.append(aParamFeature);
    }
  }
}

void ParametersPlugin_WidgetParamsMgr::updateFeaturesPart()
{
  updateItem(myFeatures, featuresItems(myParametersList));
}

void ParametersPlugin_WidgetParamsMgr::updateParametersPart()
{
  updateItem(myParameters, parametersItems(myParametersList));
}


QList<QStringList> ParametersPlugin_WidgetParamsMgr::
  featuresItems(const QList<FeaturePtr>& theFeatures) const
{
  QList<QStringList> aItemsList;
  ResultParameterPtr aParam;
  foreach(FeaturePtr aParameter, theFeatures) {
    aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParameter->firstResult());
    const std::set<std::shared_ptr<ModelAPI_Attribute>>& aRefs = aParam->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aIt;
    for(aIt = aRefs.cbegin(); aIt != aRefs.cend(); aIt++) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aReferenced.get()) {
        if (aReferenced->getKind() == ParametersPlugin_Parameter::ID()) {
          // Find referenced feature Recursive
          QList<FeaturePtr> aList;
          aList.append(aReferenced);
          QList<QStringList> aItems = featuresItems(aList);
          aItemsList.append(aItems);
        } else {
          QStringList aValNames;
          aValNames << aReferenced->data()->name().c_str();

          AttributeDoublePtr aDouble = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aAttr);
          if (aDouble.get()) {
            aValNames << aDouble->text().c_str();
            aValNames << QString::number(aDouble->value());
          } else {
            AttributeIntegerPtr aInt = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(aAttr);
            if (aInt.get()) {
              aValNames << aInt->text().c_str();
              aValNames << QString::number(aInt->value());
            }
          }
          aItemsList.append(aValNames);
        }
      }
    }
  }
  return aItemsList;
}


QList<QStringList> ParametersPlugin_WidgetParamsMgr::
  parametersItems(const QList<FeaturePtr>& theFeatures) const
{
  QList<QStringList> aItemsList;
  foreach(FeaturePtr aParameter, theFeatures) {
    ResultPtr aParam = aParameter->firstResult();
    QStringList aValues;
    aValues << aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->value().c_str();
    aValues << aParameter->string(ParametersPlugin_Parameter::EXPRESSION_ID())->value().c_str();

    AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
    aValues << QString::number(aValueAttribute->value());

    aValues << aParameter->string(ParametersPlugin_Parameter::COMMENT_ID())->value().c_str();
    aItemsList.append(aValues);
  }
  return aItemsList;
}


void ParametersPlugin_WidgetParamsMgr::onDoubleClick(const QModelIndex& theIndex)
{
  if (myDelegate->isEditable(theIndex)) {
    myTable->edit(theIndex);
  }
}

void ParametersPlugin_WidgetParamsMgr::onCloseEditor(QWidget* theEditor, 
                                                     QAbstractItemDelegate::EndEditHint theHint)
{
  FeaturePtr aFeature = myParametersList.at(myDelegate->editIndex().row());
  QTreeWidgetItem* aItem = myParameters->child(myDelegate->editIndex().row());
  int aColumn = myDelegate->editIndex().column();
  QString aText = aItem->text(aColumn);
  bool isModified = false;

  switch (aColumn) {
  case Col_Name:
    {
      AttributeStringPtr aStringAttr = aFeature->string(ParametersPlugin_Parameter::VARIABLE_ID());
      if (!aText.isEmpty()) {
        if (hasName(aText)) {
          myMessage = tr("Name %1 already exists.").arg(aText);
          if (aStringAttr->value().length() > 0)
            aItem->setText(Col_Name, aStringAttr->value().c_str());
          else 
            aItem->setText(Col_Name, NoName);
          QTimer::singleShot(50, this, SLOT(sendWarning()));
          return;
        }
        aStringAttr->setValue(aText.toStdString());
        isModified = true;
      } else {
        aItem->setText(Col_Name, aStringAttr->value().c_str());
      }
    }
    break;
  case Col_Equation:
    {
      AttributeStringPtr aStringAttr = aFeature->string(ParametersPlugin_Parameter::EXPRESSION_ID());
      if (!aText.isEmpty()) {
        if (aText != aStringAttr->value().c_str()) {
          aStringAttr->setValue(aText.toStdString());
          isModified = true;
        }
      } else {
        aItem->setText(Col_Equation, aStringAttr->value().c_str());
      }
    }
    break;
  case Col_Comment:
    {
      AttributeStringPtr aStringAttr = aFeature->string(ParametersPlugin_Parameter::COMMENT_ID());
      aStringAttr->setValue(aText.toStdString());
      isModified = true;
    }
    break;
  }

  if (!isModified)
    return;
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  ResultParameterPtr aResult = 
    std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aFeature->firstResult());
  if (aResult.get()) {
    AttributeDoublePtr aValueAttribute = 
      aResult->data()->real(ModelAPI_ResultParameter::VALUE());
    aItem->setText(Col_Result, QString::number(aValueAttribute->value()));
  }
  if (aColumn == Col_Equation)
    updateParametersPart();
  updateFeaturesPart();
}

void ParametersPlugin_WidgetParamsMgr::updateItem(QTreeWidgetItem* theItem, 
                                                  const QList<QStringList>& theFeaturesList)
{
  if (theFeaturesList.count() != theItem->childCount()) {
    if (theItem->childCount()  < theFeaturesList.count()) {
      while (theItem->childCount() != theFeaturesList.count()) 
        theItem->addChild(createNewItem());
    } else {
      while (theItem->childCount() != theFeaturesList.count()) 
        theItem->removeChild(theItem->child(theItem->childCount() - 1));
    }
  }
  int i = 0;
  foreach(QStringList aFeature, theFeaturesList) {
    int aCol = 0;
    foreach(QString aText, aFeature) {
      if (aText.length() > 0)
        theItem->child(i)->setText(aCol, aText);
      aCol++;
    }
    i++;
  }
}

FeaturePtr ParametersPlugin_WidgetParamsMgr::createParameter() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  FeaturePtr aFeature = aDoc->addFeature(ParametersPlugin_Parameter::ID());
  if (aFeature.get()) {
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return aFeature;
}


QTreeWidgetItem* ParametersPlugin_WidgetParamsMgr::createNewItem() const
{
  QStringList aValues;
  aValues << NoName;
  aValues << NoValue;

  QTreeWidgetItem* aItem = new QTreeWidgetItem(aValues);
  aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
  return aItem;
}


void ParametersPlugin_WidgetParamsMgr::onAdd()
{
  FeaturePtr aFeature = createParameter();
  if (!aFeature.get())
    return;

  QTreeWidgetItem* aItem = createNewItem();
  if (aFeature->name().length() > 0)
    aItem->setText(Col_Name, aFeature->name().c_str());
  myParameters->addChild(aItem);
  myParametersList.append(aFeature);
      
  myTable->scrollToItem(aItem);
}

QTreeWidgetItem* ParametersPlugin_WidgetParamsMgr::selectedItem() const
{
  QList<QTreeWidgetItem*> aItemsList = myTable->selectedItems();
  if (aItemsList.count() == 0)
    return 0;

  QTreeWidgetItem* aCurrentItem = aItemsList.first();
  if (aCurrentItem->parent() != myParameters)
    return 0;

  return aCurrentItem;
}


void ParametersPlugin_WidgetParamsMgr::onInsert()
{
  QTreeWidgetItem* aCurrentItem = selectedItem();
  if (!aCurrentItem)
    return;

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  FeaturePtr aNewFeature = createParameter();
  if (!aNewFeature.get())
    return;

  QTreeWidgetItem* aItem = createNewItem();
  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  if (aCurrentPos == 0) {
    aDoc->moveFeature(aNewFeature, FeaturePtr());
  } else {
    FeaturePtr aCurFeature = myParametersList.at(aCurrentPos - 1);
    aDoc->moveFeature(aNewFeature, aCurFeature);
  }
  myParametersList.insert(aCurrentPos, aNewFeature);
  myParameters->insertChild(aCurrentPos, aItem);
}

void ParametersPlugin_WidgetParamsMgr::onRemove()
{
  QTreeWidgetItem* aCurrentItem = selectedItem();
  if (!aCurrentItem)
    return;

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  FeaturePtr aCurFeature = myParametersList.at(aCurrentPos);

  QObjectPtrList anObjects;
  anObjects.append(aCurFeature);

  std::set<FeaturePtr> aDirectRefFeatures, aIndirectRefFeatures;
  ModuleBase_Tools::findReferences(anObjects, aDirectRefFeatures, aIndirectRefFeatures);

  bool doDeleteReferences = true;
  if (ModuleBase_Tools::isDeleteFeatureWithReferences(anObjects, aDirectRefFeatures, 
      aIndirectRefFeatures, this, doDeleteReferences)) {
    myParametersList.removeOne(aCurFeature);
    myParameters->removeChild(aCurrentItem);

    std::set<FeaturePtr> aFeaturesToDelete;
    if (doDeleteReferences) {
      aFeaturesToDelete = aDirectRefFeatures;
      aFeaturesToDelete.insert(aIndirectRefFeatures.begin(), aIndirectRefFeatures.end());
    }
    aDoc->removeFeature(aCurFeature);
    std::set<FeaturePtr>::const_iterator anIt = aFeaturesToDelete.begin(),
                                         aLast = aFeaturesToDelete.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = (*anIt);
      DocumentPtr aDoc = aFeature->document();
      aDoc->removeFeature(aFeature);
    }

    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED));
    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
    updateParametersFeatures();
    updateFeaturesPart();
    updateParametersPart();
  }
}

void ParametersPlugin_WidgetParamsMgr::onUp()
{
  QTreeWidgetItem* aCurrentItem = selectedItem();
  if (!aCurrentItem)
    return;

  QString aName = aCurrentItem->text(0);

  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  if (aCurrentPos == 0)
    return;
  FeaturePtr aCurFeature = myParametersList.at(aCurrentPos);

  std::string aNm = aCurFeature->data()->name();

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  FeaturePtr aa = myParametersList.at(aCurrentPos - 1);
  std::string aN = aa->data()->name();

  if (aCurrentPos == 1)
    aDoc->moveFeature(aCurFeature, FeaturePtr());
  else
    aDoc->moveFeature(aCurFeature, myParametersList.at(aCurrentPos - 2));

  myParametersList.removeOne(aCurFeature);
  myParametersList.insert(aCurrentPos - 1, aCurFeature);

  myParameters->removeChild(aCurrentItem);
  myParameters->insertChild(aCurrentPos - 1, aCurrentItem);

  selectItemScroll(aCurrentItem);

  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED));
  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED));
  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  //updateParametersFeatures();
  //updateParametersPart();
  //updateFeaturesPart();
}

void ParametersPlugin_WidgetParamsMgr::onDown()
{
  QTreeWidgetItem* aCurrentItem = selectedItem();
  if (!aCurrentItem)
    return;

  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  if (aCurrentPos == (myParametersList.count() - 1))
    return;
  FeaturePtr aCurFeature = myParametersList.at(aCurrentPos);

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();
  aDoc->moveFeature(aCurFeature, myParametersList.at(aCurrentPos + 1));

  myParametersList.removeOne(aCurFeature);
  myParametersList.insert(aCurrentPos + 1, aCurFeature);

  myParameters->removeChild(aCurrentItem);
  myParameters->insertChild(aCurrentPos + 1, aCurrentItem);

  selectItemScroll(aCurrentItem);

  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED));
  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED));
  //Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  //updateParametersFeatures();
  //updateParametersPart();
  //updateFeaturesPart();
}


bool ParametersPlugin_WidgetParamsMgr::hasName(const QString& theName) const
{
  int aCurrent = myDelegate->editIndex().row();
  int i = 0;
  foreach(FeaturePtr aFeature, myParametersList) {
    if ((i != aCurrent) && (aFeature->data()->name() == theName.toStdString()))
      return true;
    i++;
  }
  return false;
}

void ParametersPlugin_WidgetParamsMgr::sendWarning()
{
  QMessageBox::warning(this, tr("Warning"), myMessage);
}

void ParametersPlugin_WidgetParamsMgr::onSelectionChanged()
{
  QList<QTreeWidgetItem*> aItemsList = myTable->selectedItems();
  bool isParameter = false;
  foreach(QTreeWidgetItem* aItem, aItemsList) {
    if (aItem->parent() == myParameters) {
      isParameter = true;
      break;
    }
  }
  myInsertBtn->setEnabled(isParameter);
  myRemoveBtn->setEnabled(isParameter);
  myUpBtn->setEnabled(isParameter);
  myDownBtn->setEnabled(isParameter);
}

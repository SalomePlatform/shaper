// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_WidgetParamsMgr.cpp
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#include "ParametersPlugin_WidgetParamsMgr.h"
#include "ParametersPlugin_Parameter.h"

#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <QLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QPainter>

enum ColumnType {
  Col_Name,
  Col_Equation,
  Col_Result,
  Col_Comment
};

class ParametersPlugin_ItemDelegate : public QStyledItemDelegate
{
public:
  ParametersPlugin_ItemDelegate(QObject* thaParent) : 
      QStyledItemDelegate(thaParent) {}

  virtual void paint(QPainter* painter, 
    const QStyleOptionViewItem& option, 
    const QModelIndex& index ) const;
  
  //virtual QWidget* createEditor(QWidget* parent, 
  //                              const QStyleOptionViewItem& option, 
  //                              const QModelIndex& index) const;

  bool isEditable(const QModelIndex& theIndex) const;
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
  
  QStyledItemDelegate::paint(painter, option, index);

  painter->setPen(aPen);
  painter->setBrush(aBrush);
}


//QWidget* ParametersPlugin_ItemDelegate::createEditor(QWidget* parent, 
//                                                    const QStyleOptionViewItem& option, 
//                                                    const QModelIndex& index) const
//{
//  QWidget* aWgt = QStyledItemDelegate::createEditor(parent, option, index); 
//  aWgt->setMinimumSize(option.rect.width() - option.decorationSize.width(), 
//                       option.rect.height());
//  return aWgt;
//}


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
  connect(myTable, SIGNAL(doubleClicked(const QModelIndex&)),
          SLOT(onDoubleClick(const QModelIndex&)));
  //myTable->viewport()->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

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

  QToolButton* aUpBtn = new QToolButton(this);
  aUpBtn->setArrowType(Qt::DownArrow);
  aBtnLayout->addWidget(aUpBtn);
  QToolButton* aDownBtn = new QToolButton(this);
  aDownBtn->setArrowType(Qt::UpArrow);
  aBtnLayout->addWidget(aDownBtn);

  aBtnLayout->addStretch();

  QPushButton* aAddBtn = new QPushButton(tr("Add"), this);
  aBtnLayout->addWidget(aAddBtn);
  QPushButton* aInsertBtn = new QPushButton(tr("Insert"), this);
  aBtnLayout->addWidget(aInsertBtn);
  QPushButton* aRemoveBtn = new QPushButton(tr("Remove"), this);
  aBtnLayout->addWidget(aRemoveBtn);

  aLayout->addLayout(aBtnLayout);
}

QList<QWidget*> ParametersPlugin_WidgetParamsMgr::getControls() const
{
  QList<QWidget*> aList;

  return aList;
}

bool ParametersPlugin_WidgetParamsMgr::storeValueCustom() const
{
  return true;
}

bool ParametersPlugin_WidgetParamsMgr::restoreValueCustom()
{
  return true;
}

void ParametersPlugin_WidgetParamsMgr::activateCustom()
{
  FeaturePtr aFeature = feature();
  DocumentPtr aDoc = aFeature->document();
  int aNbParam = aDoc->size(ModelAPI_ResultParameter::group());
  ObjectPtr aObj;
  QTreeWidgetItem* aItem;
  ResultParameterPtr aParam;
  FeaturePtr aParamFeature;
  for (int i = 0; i < aNbParam; i++) {
    aObj = aDoc->object(ModelAPI_ResultParameter::group(), i);
    aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aObj);
    if (aParam.get()) {
      aParamFeature = ModelAPI_Feature::feature(aParam);

      QStringList aValues;
      aValues << aParamFeature->string(ParametersPlugin_Parameter::VARIABLE_ID())->value().c_str();
      aValues << aParamFeature->string(ParametersPlugin_Parameter::EXPRESSION_ID())->value().c_str();

      AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
      aValues << QString::number(aValueAttribute->value());

      //AttributeRefListPtr aParams = aParamFeature->reflist(ParametersPlugin_Parameter::ARGUMENTS_ID());
      //std::list<ObjectPtr> aList = aParams->list();
      //std::string aName;
      //std::list<ObjectPtr>::iterator aIt;
      //for(aIt = aList.begin(); aIt != aList.end(); aIt++) {
      //  aObj = (*aIt);
      //  aName = aObj->data()->name();
      //}
      aItem = new QTreeWidgetItem(aValues);
      aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      myParameters->addChild(aItem);

      myFeatureList.append(aParamFeature);

      const std::set<std::shared_ptr<ModelAPI_Attribute>>& aRefs = aParam->data()->refsToMe();
      std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aIt;
      for(aIt = aRefs.cbegin(); aIt != aRefs.cend(); aIt++) {
        FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>((*aIt)->owner());
        if (aReferenced.get()) {
          QStringList aValNames;
          aValNames << aReferenced->data()->name().c_str();

          //AttributeDoublePtr aValue = aReferenced->data()->real(SketchPlugin_Constraint::VALUE());
          //aReferenced
          aItem = new QTreeWidgetItem(aValNames);
          myFeatures->addChild(aItem);
        }
      }
    }
  }
  myFeatures->setExpanded(true);
  myParameters->setExpanded(true);
}


void ParametersPlugin_WidgetParamsMgr::onDoubleClick(const QModelIndex& theIndex)
{
  if (myDelegate->isEditable(theIndex)) {
    myTable->edit(theIndex);
    myEditingIndex = theIndex;
  }
}

void ParametersPlugin_WidgetParamsMgr::onCloseEditor(QWidget* theEditor, 
                                                     QAbstractItemDelegate::EndEditHint theHint)
{
  if (myEditingIndex.column() == Col_Equation) {
    QTreeWidgetItem* aItem = myParameters->child(myEditingIndex.row());
    QString aText = aItem->text(myEditingIndex.column());
    if (!aText.isEmpty()) {
      FeaturePtr aFeature = myFeatureList.at(myEditingIndex.row());
      AttributeStringPtr aStringAttr = aFeature->string(ParametersPlugin_Parameter::EXPRESSION_ID());
      aStringAttr->setValue(aText.toStdString());
      aFeature->execute();
      ResultParameterPtr aResult = 
        std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aFeature->firstResult());
      if (aResult.get()) {
        AttributeDoublePtr aValueAttribute = 
          aResult->data()->real(ModelAPI_ResultParameter::VALUE());
        aItem->setText(Col_Result, QString::number(aValueAttribute->value()));
      }
    }
  }
  myEditingIndex = QModelIndex();
}
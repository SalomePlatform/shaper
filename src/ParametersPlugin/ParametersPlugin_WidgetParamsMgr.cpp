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

#include "ParametersPlugin_WidgetParamsMgr.h"
#include "ParametersPlugin_Parameter.h"
#include "ParametersPlugin_Validators.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Expression.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <ModuleBase_Tools.h>

#include <Events_Loop.h>

#include <QLayout>
#include <QPushButton>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QDialogButtonBox>

enum ColumnType {
  Col_Name,
  Col_Equation,
  Col_Result,
  Col_Comment
};

const char* NoName = "<NoName>";
const char* NoValue = "<NoValue>";

/*!
 * \ingroup GUI
 * ItemDelegate object in order to redefine items behavior
 */
class ParametersPlugin_ItemDelegate : public QStyledItemDelegate
{
public:
  /// Constructor
  /// \param thaParent a parent
  ParametersPlugin_ItemDelegate(QObject* thaParent) :
      QStyledItemDelegate(thaParent) {}

  /// Redefinition of virtual method
  /// \param painter a painter object
  /// \param option the item options
  /// \param index the current index
  virtual void paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index ) const;

  /// Redefinition of virtual method
  /// \param parent a parent widget
  /// \param option the item options
  /// \param index the current index
  virtual QWidget* createEditor(QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const;

  /// Returns True if the given index is editable item
  /// \param theIndex an item index
  bool isEditable(const QModelIndex& theIndex) const;

  /// Returns currently editing index
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
  //if (!isEditable(index))
  //  painter->setBrush(Qt::lightGray);
  if (!index.parent().isValid())
    painter->setBrush(Qt::lightGray);

  painter->setPen(Qt::lightGray);
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
void ParametersPlugin_TreeWidget::closeEditor(QWidget* theEditor,
                                              QAbstractItemDelegate::EndEditHint theHint)
{
  if (theHint == QAbstractItemDelegate::EditNextItem) {
    QModelIndex aCurrent = currentIndex();
    QModelIndex aParent = model()->index(0, 0);
    int aNbRows = model()->rowCount(aParent);
    QModelIndex aIdx;
    switch (aCurrent.column()) {
    case 0:
      aIdx = model()->index(aCurrent.row(), 1, aParent);
      break;
    case 1:
      if (aCurrent.row() < (aNbRows - 1))
        aIdx = model()->index(aCurrent.row() + 1, 0, aParent);
      else {
        QTreeWidget::closeEditor(theEditor, QAbstractItemDelegate::NoHint);
        return;
      }
      break;
    case 3:
      QTreeWidget::closeEditor(theEditor, theHint);
      return;
    }
    if (aIdx.isValid()) {
      QTreeWidget::closeEditor(theEditor, QAbstractItemDelegate::NoHint);
      setCurrentIndex(aIdx);
      edit(aIdx);
      return;
    }
  }
  QTreeWidget::closeEditor(theEditor, theHint);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ParametersPlugin_WidgetParamsMgr::ParametersPlugin_WidgetParamsMgr(QWidget* theParent,
  const Config_WidgetAPI* theData)
  : ModuleBase_ModelDialogWidget(theParent, theData),
  isUpplyBlocked(false)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);

  myTable = new ParametersPlugin_TreeWidget(this);
  myTable->setColumnCount(4);
  QStringList aHeaders;
  aHeaders << translate("Name") << translate("Expression")
           << translate("Result") << translate("Comment");
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
  aNames<<translate("Parameters");
  myParameters = new QTreeWidgetItem(aNames);
  myParameters->setFlags(Qt::ItemIsEnabled);
  myTable->addTopLevelItem(myParameters);

  aNames.clear();
  aNames<<translate("Features");
  myFeatures = new QTreeWidgetItem(aNames);
  myFeatures->setFlags(Qt::ItemIsEnabled);
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

  myAddBtn = new QPushButton(translate("Add"), this);
  connect(myAddBtn, SIGNAL(clicked(bool)), SLOT(onAdd()));
  aBtnLayout->addWidget(myAddBtn);

  myInsertBtn = new QPushButton(translate("Insert"), this);
  connect(myInsertBtn, SIGNAL(clicked(bool)), SLOT(onInsert()));
  aBtnLayout->addWidget(myInsertBtn);

  myRemoveBtn = new QPushButton(translate("Remove"), this);
  connect(myRemoveBtn, SIGNAL(clicked(bool)), SLOT(onRemove()));
  aBtnLayout->addWidget(myRemoveBtn);

  aLayout->addLayout(aBtnLayout);

  onSelectionChanged();
}

void ParametersPlugin_WidgetParamsMgr::setDialogButtons(QDialogButtonBox* theButtons)
{
  ModuleBase_ModelDialogWidget::setDialogButtons(theButtons);

  QWidget* aBtnParentWgt = myOkCancelBtn->parentWidget();
  QHBoxLayout* aBtnParentLayout = dynamic_cast<QHBoxLayout*>(aBtnParentWgt->layout());

  QPushButton* aPreviewBtn = new QPushButton(translate("See preview"), aBtnParentWgt);
  aBtnParentLayout->insertWidget(0, aPreviewBtn);
  aBtnParentLayout->insertStretch(1, 1);
  connect(aPreviewBtn, SIGNAL(clicked(bool)), SLOT(onShowPreview()));
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
  Events_InfoMessage aErr;
  int aId = 0;
  foreach(FeaturePtr aFeature, myParametersList) {
    if (!aValidator.isValid(aFeature->attribute(aAttrId), aArgs, aErr)) {
      QMessageBox::warning(this, translate("Warning"), aErr.messageString().c_str());
      selectItemScroll(myParameters->child(aId));
      return false;
    }
    aId++;
  }
  // #2813 : make the current feature the latest in the document
  std::list<FeaturePtr> allFeatures = myFeature->document()->allFeatures();
  if (!allFeatures.empty()) {
    myFeature->document()->setCurrentFeature(*(allFeatures.rbegin()), true);
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
  ObjectPtr aObj;
  FeaturePtr aParamFeature;
  int aNbFeatures = aDoc->numInternalFeatures();
  for (int i = 0; i < aNbFeatures; i++) {
    aParamFeature = aDoc->internalFeature(i);
    if (aParamFeature && aParamFeature->getKind() == ParametersPlugin_Parameter::ID()) {
      myParametersList.append(aParamFeature);
    }
  }
}

void ParametersPlugin_WidgetParamsMgr::updateFeaturesPart()
{
  QList<FeaturePtr> aFeatureList;
  updateItem(myFeatures, featuresItems(myParametersList, aFeatureList));
}

void ParametersPlugin_WidgetParamsMgr::updateParametersPart()
{
  updateItem(myParameters, parametersItems(myParametersList));
  bool aIsValid = isValid();
  enableButtons(aIsValid);
}


QList<QStringList> ParametersPlugin_WidgetParamsMgr::
  featuresItems(const QList<FeaturePtr>& theFeatures, QList<FeaturePtr>& theFeatureList) const
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
      if (aReferenced.get() && (aReferenced != aParameter)) {
        if (aReferenced->getKind() == ParametersPlugin_Parameter::ID()) {
          // Find referenced feature Recursive
          QList<FeaturePtr> aList;
          aList.append(aReferenced);
          QList<QStringList> aItems = featuresItems(aList, theFeatureList);
          aItemsList.append(aItems);
        } else {
          if (!theFeatureList.contains(aReferenced)) {
            QStringList aValNames;
            aValNames << aReferenced->data()->name().c_str();

            std::string aId = aAttr->attributeType();
            if (aId == ModelAPI_AttributeDouble::typeId()) {
              AttributeDoublePtr aDouble =
                std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aAttr);
              aValNames << aDouble->text().c_str();
              aValNames << QString::number(aDouble->value());
            }
            else if (aId == ModelAPI_AttributeInteger::typeId()) {
              AttributeIntegerPtr aInt =
                std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(aAttr);
              aValNames << aInt->text().c_str();
              aValNames << QString::number(aInt->value());
            }
            else if (aId == GeomDataAPI_Point::typeId()) {
              std::shared_ptr<GeomDataAPI_Point> aPnt =
                std::dynamic_pointer_cast<GeomDataAPI_Point>(aAttr);

              QString aExpr = QString("%1,%2,%3").arg(aPnt->textX().c_str()).
                arg(aPnt->textY().c_str()).arg(aPnt->textZ().c_str());
              aValNames << aExpr;

              QString aRes = QString("%1,%2,%3").arg(aPnt->x()).arg(aPnt->y()).arg(aPnt->z());
              aValNames << aRes;
            }
            else if (aId == GeomDataAPI_Point2D::typeId()) {
              std::shared_ptr<GeomDataAPI_Point2D> aPnt =
                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aAttr);

              QString aExpr = QString("%1,%2").arg(aPnt->textX().c_str()).
                arg(aPnt->textY().c_str());
              aValNames << aExpr;

              QString aRes = QString("%1,%2").arg(aPnt->x()).arg(aPnt->y());
              aValNames << aRes;
            }
            aItemsList.append(aValNames);
            theFeatureList.append(aReferenced);
          }
        }
      }
    }
  }
  return aItemsList;
}


QList<QStringList> ParametersPlugin_WidgetParamsMgr::
  parametersItems(const QList<FeaturePtr>& theFeatures) const
{
  std::list<std::string> aArgs;
  std::string aErr;
  QList<QStringList> aItemsList;
  foreach(FeaturePtr aParameter, theFeatures) {
    ResultPtr aParam = aParameter->firstResult();
    QStringList aValues;

    std::string aName = aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->value();
    if (aName.empty()) {
      aValues << translate(NoName);
    } else
      aValues << aName.c_str();

    std::string aExpr = aParameter->string(ParametersPlugin_Parameter::EXPRESSION_ID())->value();
    if (aName.empty()) {
      aValues << translate(NoValue);
    } else
      aValues << aExpr.c_str();

    std::string aErr =
      aParameter->data()->string(ParametersPlugin_Parameter::EXPRESSION_ERROR_ID())->value();
    if (aErr.empty()) {
      AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
      aValues << QString::number(aValueAttribute->value());
    } else {
      aValues << aErr.c_str();
    }
    aValues << aParameter->string(ParametersPlugin_Parameter::COMMENT_ID())->value().c_str();
    aItemsList.append(aValues);
  }
  return aItemsList;
}


void ParametersPlugin_WidgetParamsMgr::onDoubleClick(const QModelIndex& theIndex)
{
  if (myDelegate->isEditable(theIndex)) {
    myTable->setCurrentIndex(theIndex);
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
        while (aText.indexOf(" ") != -1) {
          aText.replace(" ", "");
        }
        if (hasName(aText)) {
          myMessage = translate("Name '%1' already exists.").arg(aText);
          QTimer::singleShot(50, this, SLOT(sendWarning()));
          return;
        }
        aStringAttr->setValue(aText.toStdString());
        isModified = true;
      }
    }
    break;
  case Col_Equation:
    {
      AttributeStringPtr aStringAttr =
        aFeature->string(ParametersPlugin_Parameter::EXPRESSION_ID());
      if (!aText.isEmpty()) {
        if (aText != aStringAttr->value().c_str()) {
          aStringAttr->setValue(aText.toStdString());
          aFeature->execute();
          isModified = true;
        }
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

  if (aColumn != Col_Comment)
    updateParametersPart();
  updateFeaturesPart();

  onSelectionChanged();
}

void ParametersPlugin_WidgetParamsMgr::updateItem(QTreeWidgetItem* theItem,
                                                  const QList<QStringList>& theFeaturesList)
{
  if (theFeaturesList.count() != theItem->childCount()) {
    if (theItem->childCount()  < theFeaturesList.count()) {
      while (theItem->childCount() != theFeaturesList.count())
        theItem->addChild(createNewItem(theItem));
    } else {
      while (theItem->childCount() != theFeaturesList.count())
        theItem->removeChild(theItem->child(theItem->childCount() - 1));
    }
  }
  int i = 0;
  foreach(QStringList aFeature, theFeaturesList) {
    int aCol = 0;
    foreach(QString aText, aFeature) {
      if (aText.length() > 0) {
        theItem->child(i)->setText(aCol, aText);
        theItem->child(i)->setToolTip(aCol, aText);
      }
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


QTreeWidgetItem* ParametersPlugin_WidgetParamsMgr::createNewItem(QTreeWidgetItem* theParent) const
{
  QStringList aValues;
  aValues << translate(NoName);
  aValues << translate(NoValue);

  QTreeWidgetItem* aItem = new QTreeWidgetItem(aValues);
  if (theParent == myParameters) {
    aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    aItem->setForeground(2, Qt::darkGray);
  } else
    aItem->setFlags(Qt::NoItemFlags);
  return aItem;
}


void ParametersPlugin_WidgetParamsMgr::onAdd()
{
  FeaturePtr aFeature = createParameter();
  if (!aFeature.get())
    return;

  myParametersList.append(aFeature);
  updateParametersPart();

  QTreeWidgetItem* aItem = myParameters->child(myParameters->childCount() - 1);

  myTable->scrollToItem(aItem);
  myTable->setCurrentItem(aItem);
  myTable->editItem(aItem);

  enableButtons(false);
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

  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  if (aCurrentPos == 0) {
    aDoc->moveFeature(aNewFeature, FeaturePtr());
  } else {
    FeaturePtr aCurFeature = myParametersList.at(aCurrentPos - 1);
    aDoc->moveFeature(aNewFeature, aCurFeature);
  }
  updateParametersFeatures();
  updateParametersPart();

  myTable->scrollToItem(aCurrentItem);
  myTable->setCurrentItem(aCurrentItem);
  myTable->editItem(aCurrentItem);
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

  std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
  std::set<FeaturePtr> aFeatures;
  ModuleBase_Tools::convertToFeatures(anObjects, aFeatures);
  ModelAPI_Tools::findAllReferences(aFeatures, aReferences);

  std::set<FeaturePtr> aFeatureRefsToDelete;
  if (ModuleBase_Tools::askToDelete(aFeatures, aReferences, this, aFeatureRefsToDelete)) {
    if (!aFeatureRefsToDelete.empty())
      aFeatures.insert(aFeatureRefsToDelete.begin(), aFeatureRefsToDelete.end());
    ModelAPI_Tools::removeFeatures(aFeatures, false);

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

  int aCurrentPos = myParameters->indexOfChild(aCurrentItem);
  if (aCurrentPos == 0)
    return;
  FeaturePtr aCurFeature = myParametersList.at(aCurrentPos);

  SessionPtr aMgr = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aDoc = aMgr->activeDocument();

  if (aCurrentPos == 1)
    aDoc->moveFeature(aCurFeature, FeaturePtr());
  else
    aDoc->moveFeature(aCurFeature, myParametersList.at(aCurrentPos - 2));

  // add the updated also the feature that goes down
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED));
  static Events_ID EVENT_UPD = Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED);
  ModelAPI_EventCreator::get()->sendUpdated(myParametersList.at(aCurrentPos - 1), EVENT_UPD);
  Events_Loop::loop()->flush(EVENT_UPD);
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  updateParametersFeatures();
  updateParametersPart();
  updateFeaturesPart();

  if (aCurrentPos > 0) {
    aCurrentItem = myParameters->child(aCurrentPos - 1);
    myTable->setCurrentItem(aCurrentItem);
    selectItemScroll(aCurrentItem);
  }
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
  // add the updated also the feature that goes up
  static Events_ID EVENT_UPD = Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED);
  ModelAPI_EventCreator::get()->sendUpdated(myParametersList.at(aCurrentPos + 1), EVENT_UPD);

  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED));
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  updateParametersFeatures();
  updateParametersPart();
  updateFeaturesPart();

  if (aCurrentPos < myParameters->childCount() - 1) {
    aCurrentItem = myParameters->child(aCurrentPos + 1);
    myTable->setCurrentItem(aCurrentItem);
    selectItemScroll(aCurrentItem);
  }
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
  QMessageBox::warning(this, translate("Warning"), myMessage);
  QTreeWidgetItem* aItem = myTable->currentItem();
  if (aItem)
    myTable->editItem(aItem);
}

void ParametersPlugin_WidgetParamsMgr::onSelectionChanged()
{
  QList<QTreeWidgetItem*> aItemsList = myTable->selectedItems();
  bool aIsValid = isValid();
  if (aIsValid) {
    bool isParameter = false;
    foreach(QTreeWidgetItem* aItem, aItemsList) {
      if (aItem->parent() == myParameters) {
        isParameter = true;
        break;
      }
    }
    myInsertBtn->setEnabled(isParameter);
    //myRemoveBtn->setEnabled(isParameter);
    myUpBtn->setEnabled(isParameter);
    myDownBtn->setEnabled(isParameter);
  } else {
    myInsertBtn->setEnabled(false);
    //myRemoveBtn->setEnabled(false);
    myUpBtn->setEnabled(false);
    myDownBtn->setEnabled(false);
  }
  myRemoveBtn->setEnabled(!aItemsList.isEmpty());
}

void ParametersPlugin_WidgetParamsMgr::enableButtons(bool theEnable)
{
  myAddBtn->setEnabled(theEnable);
  if (theEnable)
    onSelectionChanged();
  else {
    myInsertBtn->setEnabled(theEnable);
    //myRemoveBtn->setEnabled(theEnable);
    myUpBtn->setEnabled(theEnable);
    myDownBtn->setEnabled(theEnable);
  }
  myOkCancelBtn->button(QDialogButtonBox::Ok)->setEnabled(theEnable);
}

bool ParametersPlugin_WidgetParamsMgr::isValid()
{
  QTreeWidgetItem* aItem;
  for(int i = 0; i < myParameters->childCount(); i++) {
    aItem = myParameters->child(i);
    if ((aItem->text(Col_Name) == NoName) ||
        (aItem->text(Col_Equation) == translate(NoValue)) ||
        (!ModelAPI_Expression::isVariable(aItem->text(Col_Name).toStdString())) ) {
      return false;
    }
  }
  return true;
}

void ParametersPlugin_WidgetParamsMgr::showEvent(QShowEvent* theEvent)
{
  ModuleBase_ModelDialogWidget::showEvent(theEvent);
  SessionPtr aMgr = ModelAPI_Session::get();
  isUpplyBlocked = aMgr->isAutoUpdateBlocked();
  aMgr->blockAutoUpdate(true);
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(aLoop->eventByName(EVENT_AUTOMATIC_RECOMPUTATION_DISABLE));
}

void ParametersPlugin_WidgetParamsMgr::hideEvent(QHideEvent* theEvent)
{
  ModuleBase_ModelDialogWidget::hideEvent(theEvent);
  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->blockAutoUpdate(isUpplyBlocked);
}

void ParametersPlugin_WidgetParamsMgr::onShowPreview()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->blockAutoUpdate(false);
  aMgr->blockAutoUpdate(true);
}
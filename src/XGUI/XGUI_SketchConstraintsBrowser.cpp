// Copyright (C) 2014-2026  CEA/DEN, EDF R&D
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

#include "XGUI_SketchConstraintsBrowser.h"
#include "XGUI_Tools.h"
#include "XGUI_DataModel.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultField.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_ITreeNode.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_IModule.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Workshop.h>
#include <ModuleBase_WidgetFactory.h>
#include <XGUI_Displayer.h>

#include <QLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QApplication>
#include <QGroupBox>
#include <QPainter>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>

#include <unordered_map>

#ifdef DEBUG_INDXES
#include <QToolTip>
#endif


#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif

// Type of columns
enum ColumnType {
  Col_Icon,
  Col_Constraint,
  Col_Primitive,
  Col_Value
};

namespace
{
  QModelIndex GetIndex(QModelIndex theChildIndex)
  {
    auto aParent = theChildIndex.parent();
    if (aParent.isValid())
      return aParent.model()->index(aParent.row(), 1);
    else
      return QModelIndex();
  }

  // Retrurn name for constraint attribute
  QString GetName(const AttributePtr& theAttribute)
  {
    QString aName;

    if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId())
      return aName;

    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

    auto anObj = aRefAttr->object();
    auto anAttr = aRefAttr->attr();
    if (anAttr)
    {
      FeaturePtr anAttrA = ModelAPI_Feature::feature(anAttr->owner());
      aName += QString::fromStdWString(anAttrA->name());
      aName += "/";
      aName += QString::fromStdString(anAttr->id());
    }
    else if (anObj)
    {
      FeaturePtr anAttrA = ModelAPI_Feature::feature(anObj);
      aName += QString::fromStdWString(anAttrA->name());
    }

    return aName;
  }

  std::pair<std::string, std::string> FromSketchKindToName(const std::string& theKind)
  {
    const std::string& aType = theKind;
    if (aType == "SketchConstraintCoincidence" ||
      aType == "SketchConstraintCoincidenceInternal")
      return { "Coincidence", "coincedence.png" };
    else if (aType == "SketchConstraintRigid")
      return { "Fixed", "fixed.png" };
    else if (aType == "SketchConstraintHorizontal")
      return { "Horizontal", "horisontal.png" };
    else if (aType == "SketchConstraintVertical")
      return { "Vertical", "vertical.png" };
    else if (aType == "SketchConstraintAngle")
      return { "Angle", "angle_constr.png" };
    else if (aType == "SketchConstraintDistance")
      return { "Distance", "distance.png" };
    else if (aType == "SketchConstraintDistanceHorizontal")
      return { "Horizontal distance", "distance_h.png" };
    else if (aType == "SketchConstraintDistanceVertical")
      return { "Vertical distance", "distance_v.png" };
    else if (aType == "SketchConstraintEqual")
      return { "Equal", "equal.png" };
    else if (aType == "SketchConstraintLength")
      return { "Length", "length.png" };
    else if (aType == "SketchConstraintMiddle")
      return { "Middle point", "middlepoint.png" };
    else if (aType == "SketchConstraintMirror")
      return { "Mirror objects", "mirror.png" };
    else if (aType == "SketchConstraintParallel")
      return { "Parallel", "parallel.png" };
    else if (aType == "SketchConstraintPerpendicular")
      return { "Perpendicular", "perpendicular.png" };
    else if (aType == "SketchConstraintRadius")
      return { "Radius", "radius_constr.png" };
    else if (aType == "SketchConstraintCollinear")
      return { "Collinear", "collinear.png" };
    else if (aType == "SketchConstraintTangent")
      return { "Tangent", "tangent.png" };
    return { "", "" };
  }

  std::string GetIconPath(const std::string& theKind)
  {
    std::string aFile;
    char* anEnv = getenv("SHAPER_ROOT_DIR");
    if (anEnv) {
      aFile = std::string(anEnv) +
        FSEP + "share" + FSEP + "salome" + FSEP + "resources" + FSEP + "shaper" + FSEP + "icons" + FSEP + "Sketch";
    }
    else {
      anEnv = getenv("CADBUILDER_ROOT_DIR");
      if (anEnv)
        aFile = std::string(anEnv) + FSEP + "plugins" + FSEP + "icons" + FSEP + "Sketch";
    }

    aFile += FSEP;
    aFile += FromSketchKindToName(theKind).second;
    return aFile;
  }
}

/*!
 * \ingroup GUI
 * ItemDelegate object in order to redefine items behavior
 */
class XGUI_ConstraintsItemDelegate : public QStyledItemDelegate
{
public:
  /// Constructor
  /// \param theParent a parent
  XGUI_ConstraintsItemDelegate(QObject* theParent) :
    QStyledItemDelegate(theParent) {}

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

  // Return current state for TreeItem
  bool GetIsActive(const QModelIndex& index) const;

  // Modify state item
  void SetIsActive(QModelIndex& theIndex, bool theIsActive);

  /// Returns currently editing index
  QModelIndex editIndex() const { return myEditingIdx; }

protected:
  /// Redefinition of virtual method
  /// \param option the item options
  /// \param index the current index
  virtual void initStyleOption(QStyleOptionViewItem* option,
                               const QModelIndex& index) const;

private:
  mutable QModelIndex myEditingIdx;
};

// Implement

bool XGUI_ConstraintsItemDelegate::GetIsActive(const QModelIndex& index) const
{
  if (!index.parent().isValid())
    return true;

  auto aModel = index.model();
  auto anIndexForCheck = aModel->index(index.row(), 1);
  bool myIsActive = index.parent().child(index.row(), 1).data(Qt::UserRole + 1).toBool();
  return myIsActive;
}

void XGUI_ConstraintsItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
  if (index.parent().isValid())
  {
    bool myIsActive = GetIsActive(index);
    QPalette palette = QApplication::palette();
    QColor textColor = palette.color(QPalette::WindowText);
    option->palette.setBrush(QPalette::ColorRole::Text, myIsActive ? textColor : Qt::darkGray);
  }

  QStyledItemDelegate::initStyleOption(option, index);
}

void XGUI_ConstraintsItemDelegate::SetIsActive(QModelIndex& theIndex, bool theIsActive)
{
  bool aBool = theIndex.model()->data(theIndex, Qt::UserRole + 1).toBool();
  theIndex.model()->data(theIndex, Qt::UserRole + 1).setValue(!aBool);
}

QWidget* XGUI_ConstraintsItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  myEditingIdx = index;
  return QStyledItemDelegate::createEditor(parent, option, index);
}

bool XGUI_ConstraintsItemDelegate::isEditable(const QModelIndex& theIndex) const
{
  QModelIndex aParent = theIndex.parent();
  if (aParent.isValid() && !theIndex.data(0).isNull() && theIndex.column() == 3)
    return true;

  return false;
}

void XGUI_ConstraintsViewTree::closeEditor(QWidget* theEditor,
                                           QAbstractItemDelegate::EndEditHint theHint)
{
  if (theHint == QAbstractItemDelegate::EditNextItem) {
    QModelIndex aCurrent = currentIndex();
    QModelIndex aParent = model()->index(0, 0);
    int aNbRows = model()->rowCount(aParent);
    QModelIndex aIdx;
    if (aCurrent.column() == 3) {
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

//********************************************************************
XGUI_SketchConstraintsBrowser::XGUI_SketchConstraintsBrowser(QWidget* theParent, XGUI_Workshop* theWorkshop)
    : QWidget(theParent), myWorkshop(theWorkshop)
{
  // Attempt create Tree View
  myViewTree = new XGUI_ConstraintsViewTree(this);
  myViewTree->setColumnCount(4);
  QStringList aHeaders;
  aHeaders << "" << tr("Constraint") << tr("Primitives")
    << tr("Parameter");

  myViewTree->setHeaderLabels(aHeaders);
  myViewTree->setColumnWidth(Col_Icon, 40);
  myViewTree->setColumnWidth(Col_Constraint, 160);
  myViewTree->setColumnWidth(Col_Primitive, 140);
  myViewTree->setColumnWidth(Col_Value, 40);

  myViewTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  myViewTree->setSelectionBehavior(QAbstractItemView::SelectRows);
  myViewTree->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(myViewTree, SIGNAL(doubleClicked(const QModelIndex&)),
    SLOT(onDoubleClick(const QModelIndex&)));
  connect(myViewTree, SIGNAL(itemSelectionChanged()), SLOT(onSelectionChanged()));

  myDelegate = new XGUI_ConstraintsItemDelegate(myViewTree);

  myViewTree->setItemDelegate(myDelegate);

  QPalette aTreePalet = myViewTree->palette();
  QColor aTreeBack = aTreePalet.color(QPalette::Base);

  QPalette aPalet;
  aPalet.setColor(QPalette::Base, aTreeBack);
  aPalet.setColor(QPalette::Window, aTreeBack);
  myViewTree->setPalette(aTreePalet);

  connect(myViewTree, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
    SLOT(onContextMenuRequested(QContextMenuEvent*)));

  myExtInfo = new QCheckBox(tr("Extended Information"));
  myExtInfo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  myExtInfo->setChecked(true);

  // Connect for show/hide extended information
  connect(myExtInfo, SIGNAL(toggled(bool)), this, SLOT(SelectStateChanged(bool)));
  ModuleBase_Tools::adjustMargins(myExtInfo);

  myLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(myLayout);
  myLayout->setSpacing(0);

  myLayout->addWidget(myExtInfo);
  myLayout->addWidget(myViewTree);

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

XGUI_SketchConstraintsBrowser::~XGUI_SketchConstraintsBrowser()
{
  // For avoid crashes after reopen sketch
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->removeListener(this);
}

void XGUI_SketchConstraintsBrowser::SelectStateChanged(bool /*theState*/)
{
  // Add process for show columns in edit mode
  bool isShowExtInfo = !myExtInfo->isChecked();
  myViewTree->setColumnHidden(Col_Primitive, isShowExtInfo);
  myViewTree->setColumnHidden(Col_Value, isShowExtInfo);
}

// bad first param! Make more easy
bool XGUI_SketchConstraintsBrowser::UpdateTree(const std::vector<std::pair<FeaturePtr, std::vector<AttributePtr>>>& theList)
{
  // Save the expand state so that after the update, all the items will retain their previous expand/collapse state.
  std::unordered_map<std::string, bool> aPrevExpandState;
  QTreeWidgetItemIterator anIter(myViewTree);
  while (*anIter) 
  {
    QTreeWidgetItem *item = *anIter;
    if (item->parent() == nullptr)
    {
      auto aName = (item)->data(1, 0).toString();
      aPrevExpandState[aName.toStdString()] = myViewTree->isItemExpanded(item);
    }
    ++anIter;
  }
  myViewTree->clear();
  myConstrs.clear();

  // Prepare all groups of constraints
  for (const auto& anElemConstr : theList)
  {
    myConstrs[FromSketchKindToName(anElemConstr.first->getKind()).first].push_back({ anElemConstr.first, anElemConstr.second });
  }

  int aRow = 0;
  for (const auto& line : myConstrs)
  {
    //Get icon for group
    std::string aFile = GetIconPath(line.second.front().Feature->getKind());

    if (line.second[0].Attributes.size() == 0)
      continue;

    QTreeWidgetItem* anElem = new QTreeWidgetItem(myViewTree);
    anElem->setFlags(Qt::ItemIsEnabled);
    anElem->setText(Col_Constraint, QString::fromStdString(line.first));
    anElem->setIcon(Col_Icon, QIcon(QString::fromStdString(aFile)));

    //set expand
    bool isToExpand{true};
    if(aPrevExpandState.find(line.first) != aPrevExpandState.end())
    {
      isToExpand = aPrevExpandState[line.first];
    }
    anElem->setExpanded(isToExpand);

    auto aStart = line.second.begin();
    for (; aStart != line.second.end(); ++aStart)
    {
      FeatStruct aFeatStruct;
      //
      for (const auto& anElemConstr : theList)
      {
        if (anElemConstr.first == (*aStart).Feature)
        {
          aFeatStruct.Feature = anElemConstr.first;
          aFeatStruct.Attributes = anElemConstr.second;
          ++aRow;
          break;
        }
      }
      //
      QTreeWidgetItem* aSubElem = new QTreeWidgetItem(anElem);
      aSubElem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      aSubElem->setText(Col_Constraint, QString::fromStdWString((*aStart).Feature->name()));
      aSubElem->setData(Col_Constraint, Qt::UserRole + 1, (*aStart).Feature->boolean("ConstraintState")->value()); // Store state of constraints true - activated, false - suppressed

      QString aPrimitives;
      aPrimitives = GetName(aFeatStruct.Attributes[0]);
      if (aFeatStruct.Attributes.size() == 2)
      {
        aPrimitives += "\n";
        aPrimitives += GetName(aFeatStruct.Attributes[1]);
      }

      aSubElem->setText(Col_Primitive, aPrimitives);
      if ((*aStart).Feature->real("ConstraintValue"))
      {
        if ((*aStart).Feature->real("AngleValue"))
          aSubElem->setData(Col_Value, Qt::EditRole, QString::number((*aStart).Feature->real("AngleValue")->value(),'f', 4));
        else if ((*aStart).Feature->real("DistanceValue"))
          aSubElem->setData(Col_Value, Qt::EditRole, QString::number((*aStart).Feature->real("DistanceValue")->value(),'f', 4));
        else
          aSubElem->setData(Col_Value, Qt::EditRole, QString::number((*aStart).Feature->real("ConstraintValue")->value(),'f', 4));
      }
      anElem->addChild(aSubElem);
    }

    myViewTree->addTopLevelItem(anElem);
  }
  return true;
}

//******************************************************
void XGUI_SketchConstraintsBrowser::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)
    || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED))
  {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    foreach(ObjectPtr anObjectC, aObjects)
    {
      auto aCreatedFeature = ModelAPI_Feature::feature(anObjectC);
      if (aCreatedFeature && aCreatedFeature->getKind() == "Sketch")
      {
        // It's for update constraints after creating or updating
        emit deleteConstraints();
        break;
      }
    }
  }
}

//***************************************************
void XGUI_SketchConstraintsBrowser::initialize(ModuleBase_ITreeNode* theRoot)
{
  QItemSelectionModel* aSelMod = myViewTree->selectionModel();
  connect(aSelMod, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

//***************************************************
void XGUI_SketchConstraintsBrowser::onContextMenuRequested(QContextMenuEvent* theEvent)
{
  ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
  //allow appear of context menu only if no child sketch-operation is active
  bool isAllowContextMenu{myWorkshop->operationMgr()->previousOperation(aCurOperation) == NULL};
  if(isAllowContextMenu)
  {
    //close editor if it was activated
    if(IsInEditMode())
    {
      CloseEditor();
    }
    QModelIndexList aIndexes;
    QObjectPtrList aSelectedData = selectedObjects(&aIndexes);
    bool toEnable = true;

    foreach(QAction* aCmd, actions()) {
      aCmd->setEnabled(toEnable);
    }
    emit contextMenuRequested(theEvent);
  }
}

//***************************************************
bool XGUI_SketchConstraintsBrowser::IsInEditMode()
{
  QTreeWidgetItemIterator anIter(myViewTree);
  while (*anIter) {
    if(myViewTree->isPersistentEditorOpen((*anIter), Col_Value)){
      return true;
    }
    ++anIter;
  }
  return false;
}

//***************************************************
void XGUI_SketchConstraintsBrowser::CloseEditor()
{
  if(IsInEditMode())
  {
    // Close editor
    QTreeWidgetItemIterator anIter(myViewTree);
    while (*anIter) {
      myViewTree->closePersistentEditor((*anIter), Col_Value);
      ++anIter;
    }
    myExtInfo->setChecked(myLastState);

    // Send signals for apply updates
    emit editValues();
  }
}

//***************************************************
void XGUI_SketchConstraintsBrowser::onEditItem()
{
  myLastState = myExtInfo->isChecked();
  myExtInfo->setChecked(true);

  QObjectPtrList aSelectedData = selectedObjects();
  if (aSelectedData.size() > 0) {
    ObjectPtr anObject = aSelectedData.first();
    if (anObject.get()) {  // Selection happens in TreeView
      // check whether the object can be renamed. There should not be parts which are not loaded
      std::set<FeaturePtr> aFeatures;
      aFeatures.insert(ModelAPI_Feature::feature(anObject));
      if (!XGUI_Tools::canRemoveOrRename((QWidget*)parent(), aFeatures))
        return;

      // Find index which corresponds the feature
      QModelIndex aIndex;
      foreach(QModelIndex aIdx, selectedIndexes()) {
        if (aIdx.column() == Col_Value) {
            aIndex = aIdx;
            if (aIndex.isValid()) {
              myViewTree->setCurrentIndex(aIndex);
              auto aData = aIndex.data(0);
              if (!aData.isNull())
                myViewTree->openPersistentEditor(myViewTree->currentItem(), Col_Value);
            }
        }
      }
    }
  }
  else
  {
    QTreeWidgetItemIterator anIter(myViewTree);
    while (*anIter) {
      auto aData = (*anIter)->data(Col_Value, 0);
      if (!aData.isNull())
        myViewTree->openPersistentEditor((*anIter), Col_Value);
      ++anIter;
    }
  }
}

void XGUI_SketchConstraintsBrowser::onDeactivateItems()
{
  bool isActivate = true;
  std::vector<FeaturePtr> aFeaturesMod;
  // This Check need on request COntext Menu step!
  foreach(QModelIndex aIdx, selectedIndexes())
  {
    if (aIdx.isValid() && aIdx.column() == Col_Constraint) {
      auto aBool = aIdx.data(Qt::UserRole + 1).toBool();
      if (aBool)
      {
        isActivate = false;
        break;
      }
    }
  }

  QObjectPtrList aSelectedData = selectedObjects();
  if (aSelectedData.size() > 0) {
    ObjectPtr anObject = aSelectedData.first();
    if (anObject.get()) {  // Selection happens in TreeView
      // check whether the object can be renamed. There should not be parts which are not loaded
      std::set<FeaturePtr> aFeatures;
      aFeatures.insert(ModelAPI_Feature::feature(anObject));
      if (!XGUI_Tools::canRemoveOrRename((QWidget*)parent(), aFeatures))
        return;

      // Find index which corresponds the feature
      foreach(QModelIndex aIdx, selectedIndexes()) {
          auto aParent = GetIndex(aIdx);
          if (aParent.isValid() && aIdx.isValid() && aIdx.column() == Col_Constraint) {
            myViewTree->setCurrentIndex(aIdx);
            myViewTree->currentItem()->setData(Col_Constraint, Qt::UserRole + 1, isActivate);

            auto aFeat = myConstrs[aParent.data().toString().toStdString()].at(aIdx.row()).Feature;
            aFeaturesMod.push_back(aFeat);
          }
      }
    }
  }

  emit deactivate(isActivate, aFeaturesMod);
}

//***************************************************
void XGUI_SketchConstraintsBrowser::setObjectsSelected(const QObjectPtrList& theObjects)
{
  QItemSelectionModel* aSelectModel = myViewTree->selectionModel();
  QModelIndexList aIndexes = aSelectModel->selectedIndexes();
  if (theObjects.size() == 0) {
    bool aIsBlock = aSelectModel->blockSignals(true);
    aSelectModel->clear();
    aSelectModel->blockSignals(aIsBlock);
    foreach(QModelIndex aIdx, aIndexes) {
      myViewTree->update(aIdx);
    }
    return;
  }
}

//***************************************************
void XGUI_SketchConstraintsBrowser::onSelectionChanged(const QItemSelection& theSelected,
                                             const QItemSelection& theDeselected)
{
  onSelectionChanged();
}

//***************************************************
void XGUI_SketchConstraintsBrowser::onSelectionChanged()
{
  emit selectionChanged();
}

//***************************************************
QObjectPtrList XGUI_SketchConstraintsBrowser::selectedObjects(QModelIndexList* theIndexes) const
{
  QObjectPtrList aList;
  QModelIndexList aIndexes = selectedIndexes();

  foreach(QModelIndex aIdx, aIndexes) {
    if (aIdx.column() == Col_Constraint) {
      QModelIndex aParentData = GetIndex(aIdx);
      if (!aParentData.isValid())
        continue;

      std::string aData = aParentData.data().toString().toStdString();
      ObjectPtr aObject = myConstrs.at(aData).at(aIdx.row()).Feature;

      auto anAttrs = myConstrs.at(aParentData.data().toString().toStdString()).at(aIdx.row()).Attributes;
      if (aObject) {
        if (!aList.contains(aObject))
        {
          aList.append(aObject);

          // Add related primitives to list
          for (int anIndex = 0; anIndex < anAttrs.size(); ++anIndex)
          {
            if (anAttrs[anIndex]->attributeType() == ModelAPI_AttributeRefAttr::typeId())
            {
              AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttrs[anIndex]);
              if (!aRefAttr->attr())
              {
                auto aFObj = aRefAttr->object();

                if (!aList.contains(aFObj))
                {
                  aList.append(aFObj);
                }
              }
              else
              {
                aList.append(aRefAttr->attr()->owner());
              }
            }
          }

          if (theIndexes)
            theIndexes->append(aIdx);
        }
      }
    }
  }
  return aList;
}

//***************************************************
QObjectPtrList XGUI_SketchConstraintsBrowser::selectedConstraints(QModelIndexList* theIndexes) const
{
  QObjectPtrList aList;
  QModelIndexList aIndexes = selectedIndexes();

  foreach(QModelIndex aIdx, aIndexes) {
    if (aIdx.column() == Col_Constraint) {
      QModelIndex aParentData = GetIndex(aIdx);
      if (!aParentData.isValid())
        continue;

      std::string aData = aParentData.data().toString().toStdString();
      ObjectPtr aObject = myConstrs.at(aData).at(aIdx.row()).Feature;
      if (aObject) {
        if (!aList.contains(aObject))
        {
          aList.append(aObject);

          if (theIndexes)
            theIndexes->append(aIdx);
        }
      }
    }
  }
  return aList;
}

//***************************************************
void XGUI_SketchConstraintsBrowser::onDoubleClick(const QModelIndex& theIndex)
{
  ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
  //allow edit only if no child sketch-operation is active
  bool isAllowEdit{myWorkshop->operationMgr()->previousOperation(aCurOperation) == NULL};
  if (isAllowEdit && myDelegate->isEditable(theIndex)) {
    myViewTree->setCurrentIndex(theIndex);
    myViewTree->openPersistentEditor(myViewTree->currentItem(), Col_Value);
    onEditItem();
  }
}

void XGUI_SketchConstraintsBrowser::resizeEvent(QResizeEvent* theEvent)
{
  QWidget::resizeEvent(theEvent);
  emit sizeChanged();
}

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

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Tools.h"
#include "XGUI_DataModel.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultField.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_ITreeNode.h>

#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <QLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QApplication>

#ifdef DEBUG_INDXES
#include <QToolTip>
#endif

/// Width of second column (minimum acceptable = 27)
#define FIRST_COL_WIDTH 20
#define SECOND_COL_WIDTH 30

/**
* \ingroup GUI
* Tree item delegate for definition of data in column items editor
*/
class XGUI_TreeViewItemDelegate: public QStyledItemDelegate
{
public:
  /// Constructor
  /// \param theParent a parent of the delegate
  XGUI_TreeViewItemDelegate(XGUI_DataTree* theParent):QStyledItemDelegate(theParent),
    myTreedView(theParent) {}

  /// Set data for item editor (name of the item)
  /// \param editor a widget of editor
  /// \param index the tree item index
  virtual void	setEditorData ( QWidget* editor, const QModelIndex& index ) const
  {
    QLineEdit* aEditor = dynamic_cast<QLineEdit*>(editor);
    if (aEditor) {
      XGUI_DataModel* aModel = myTreedView->dataModel();
      ObjectPtr aObj = aModel->object(index);
      if (aObj.get() != NULL) {
        aEditor->setText(aObj->data()->name().c_str());
        return;
      }
    }
    QStyledItemDelegate::setEditorData(editor, index);
  }

private:
  XGUI_DataTree* myTreedView;
};


XGUI_DataTree::XGUI_DataTree(QWidget* theParent)
    : QTreeView(theParent)
{
  setHeaderHidden(true);
  setTreePosition(1);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  setItemDelegateForColumn(1, new XGUI_TreeViewItemDelegate(this));

  connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
    SLOT(onDoubleClick(const QModelIndex&)));
}

XGUI_DataTree::~XGUI_DataTree()
{
}

XGUI_DataModel* XGUI_DataTree::dataModel() const
{
  return static_cast<XGUI_DataModel*>(model());
}

void XGUI_DataTree::contextMenuEvent(QContextMenuEvent* theEvent)
{
  emit contextMenuRequested(theEvent);
}

void XGUI_DataTree::commitData(QWidget* theEditor)
{
  static int aEntrance = 0;
  if (aEntrance == 0) {
    // We have to check number of enter and exit of this function because it can be called
    // recursively by Qt in order to avoid double modifying of a data
    aEntrance = 1;
    QLineEdit* aEditor = dynamic_cast<QLineEdit*>(theEditor);
    if (aEditor) {
      QString aName = aEditor->text();
      QModelIndexList aIndexList = selectionModel()->selectedIndexes();
      XGUI_DataModel* aModel = dataModel();
      ObjectPtr aObj = aModel->object(aIndexList.first());

      if (XGUI_Tools::canRename(aObj, aName)) {
        SessionPtr aMgr = ModelAPI_Session::get();
        aMgr->startOperation("Rename");
        aObj->data()->setName(qPrintable(aName));
        aMgr->finishOperation();
      }
    }
  }
  aEntrance = 0;
}

void XGUI_DataTree::clear()
{
  dataModel()->clear();
  reset();
}

void XGUI_DataTree::resizeEvent(QResizeEvent* theEvent)
{
  QTreeView::resizeEvent(theEvent);
  QSize aSize = theEvent->size();
  if (aSize.isValid()) {
    setColumnWidth(0, FIRST_COL_WIDTH);
    setColumnWidth(1, aSize.width() - SECOND_COL_WIDTH - FIRST_COL_WIDTH - 10);
    setColumnWidth(2, SECOND_COL_WIDTH);
  }
}

#ifdef DEBUG_INDXES
void XGUI_DataTree::mousePressEvent(QMouseEvent* theEvent)
{
  QTreeView::mousePressEvent(theEvent);
  if (theEvent->button() != Qt::MidButton)
    return;
  QModelIndex aInd = indexAt(theEvent->pos());
  QString aTxt =
    QString("r=%1 c=%2 p=%3").arg(aInd.row()).arg(aInd.column()).arg((long)aInd.internalPointer());

  QModelIndex aPar = aInd.parent();
  QString aTxt1 =
    QString("r=%1 c=%2 p=%3").arg(aPar.row()).arg(aPar.column()).arg((long)aPar.internalPointer());
  QToolTip::showText(theEvent->globalPos(), aTxt + '\n' + aTxt1);
}
#endif

void XGUI_DataTree::mouseReleaseEvent(QMouseEvent* theEvent)
{
  QTreeView::mouseReleaseEvent(theEvent);
#ifdef DEBUG_INDXES
  if (theEvent->button() != Qt::MidButton)
    return;
  QToolTip::hideText();
#endif
  if (theEvent->button() == Qt::LeftButton) {
    QModelIndex aInd = indexAt(theEvent->pos());
    if (aInd.column() == 0)
      processEyeClick(aInd);
  }
}

void XGUI_DataTree::processHistoryChange(const QModelIndex& theIndex)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  // When operation is opened then we can not change history
  if (aMgr->isOperation())
    return;
  XGUI_DataModel* aModel = dataModel();
  if (aModel->flags(theIndex) == 0)
    return;
  ObjectPtr aObj = aModel->object(theIndex);

  DocumentPtr aDoc = aMgr->activeDocument();

  std::string aOpName = tr("History change").toStdString();
  if (aObj.get()) {
    if (aObj->document() != aDoc)
      return;
    aMgr->startOperation(aOpName);
    aDoc->setCurrentFeature(std::dynamic_pointer_cast<ModelAPI_Feature>(aObj), true);
    aMgr->finishOperation();
  } else {
    // Ignore clicks on folders outside current document
    if ((theIndex.internalId() == 0) && (aDoc != aMgr->moduleDocument()))
      // Clicked folder under root but active document is another
      return;
    if ((theIndex.internalId() != 0) && (aDoc != aModel->document(theIndex)))
      // Cliced not on active document folder
      return;

    aMgr->startOperation(aOpName);
    aDoc->setCurrentFeature(FeaturePtr(), true);
    aMgr->finishOperation();
  }
  QModelIndex aParent = theIndex.parent();
  int aSize = aModel->rowCount(aParent);
  for (int i = 0; i < aSize; i++) {
    update(aModel->index(i, 0, aParent));
    update(aModel->index(i, 1, aParent));
    update(aModel->index(i, 2, aParent));
  }
  XGUI_ObjectsBrowser* aObjBrowser = qobject_cast<XGUI_ObjectsBrowser*>(parent());
  aObjBrowser->workshop()->displayer()->updateViewer();
}

void XGUI_DataTree::processEyeClick(const QModelIndex& theIndex)
{
  XGUI_ObjectsBrowser* aObjBrowser = qobject_cast<XGUI_ObjectsBrowser*>(parent());
  XGUI_DataModel* aModel = dataModel();
  ObjectPtr aObj = aModel->object(theIndex);
  if (aObj.get()) {
    std::set<ObjectPtr> anObjects;
    anObjects.insert(aObj);

    bool hasHiddenState = aModel->hasHiddenState(theIndex);
    if (aObjBrowser && hasHiddenState && !aObjBrowser->workshop()->prepareForDisplay(anObjects))
      return;
    if (hasHiddenState) { // #issue 2335(hide all faces then show solid problem)
      if (aObj->isDisplayed())
        aObj->setDisplayed(false);
      aObj->setDisplayed(true);
    }
    else
      aObj->setDisplayed(!aObj->isDisplayed());

    // Update list of selected objects because this event happens after
    // selection event in object browser
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
    update(theIndex);
    if (aObjBrowser) {
      aObjBrowser->onSelectionChanged();
    }
  }
}

void XGUI_DataTree::onDoubleClick(const QModelIndex& theIndex)
{
  switch (theIndex.column()) {
  case 2:
    processHistoryChange(theIndex);
    break;
  }
}


//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ActiveDocLbl::XGUI_ActiveDocLbl(const QString& theText, QWidget* theParent )
  : QLabel(theText, theParent),
  myPreSelectionStyle(""),
  myNeutralStyle(""),
  mySelectionStyle(""),
  myIsSelected(false)
{
}

void XGUI_ActiveDocLbl::setTreeView(QTreeView* theView)
{
  myTreeView = theView;
  setFont(myTreeView->font());

  QPalette aPalet = myTreeView->palette();
  QColor aHighlight = aPalet.highlight().color();
  QColor aHighlightText = aPalet.highlightedText().color();

  myPreSelectionStyle = "QLabel {background-color: ";
  myPreSelectionStyle += aHighlight.lighter(170).name() + "}";
  //myPreSelectionStyle += "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 " +
  //  aHighlight.lighter(170).name() + ");";
  //myPreSelectionStyle += "border: 1px solid lightblue; border-radius: 2px }";

  QString aName = aPalet.color(QPalette::Base).name();
  myNeutralStyle = "QLabel { border: 1px solid " + aName + " }";


#if (!defined HAVE_SALOME) && (defined WIN32)
  mySelectionStyle = "QLabel {background-color: ";
  mySelectionStyle += "rgb(205, 232, 255); ";
  //mySelectionStyle += "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(236, 245, 255)";
  //mySelectionStyle += ", stop:1 rgb(208, 229, 255));";
  //mySelectionStyle += "border: 1px solid rgb(132, 172, 221); border-radius: 2px }";
  mySelectionStyle += "border: 1px solid rgb(153, 209, 255) }";
#else
  mySelectionStyle = "QLabel {background-color: " + aHighlight.name();
  mySelectionStyle += "; color : " + aHighlightText.name() + "}";
#endif

  myTreeView->viewport()->installEventFilter(this);
}


#if (!defined HAVE_SALOME) && (defined WIN32)
bool XGUI_ActiveDocLbl::event(QEvent* theEvent)
{
  switch (theEvent->type()) {
    case QEvent::Enter:
      if (!myIsSelected)
        setStyleSheet(myPreSelectionStyle);
      break;
    case QEvent::Leave:
      if (!myIsSelected)
        setStyleSheet(myNeutralStyle);
      break;
  }
  return QLabel::event(theEvent);
}
#endif

bool XGUI_ActiveDocLbl::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myTreeView->viewport()) {
    if (theEvent->type() == QEvent::MouseButtonRelease)
      unselect();
  }
  return QLabel::eventFilter(theObj, theEvent);
}

static bool MYClearing = false;
void XGUI_ActiveDocLbl::mouseReleaseEvent( QMouseEvent* e)
{
  MYClearing = true;
  myIsSelected = true;
  setStyleSheet(mySelectionStyle);
  // We can not block signals because on
  // clear selection the View state will not be updated
  myTreeView->clearSelection();
  QLabel::mouseReleaseEvent(e);
  MYClearing = false;
}

void XGUI_ActiveDocLbl::unselect()
{
  if (!MYClearing) {
    myIsSelected = false;
    setStyleSheet(myNeutralStyle);
  }
}


//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent, XGUI_Workshop* theWorkshop)
    : QWidget(theParent), myDocModel(0), myWorkshop(theWorkshop)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->setSpacing(0);

  QWidget* aLabelWgt = new QWidget(this);
  aLabelWgt->setAutoFillBackground(true);

  aLayout->addWidget(aLabelWgt);
  QHBoxLayout* aLabelLay = new QHBoxLayout(aLabelWgt);
  ModuleBase_Tools::zeroMargins(aLabelLay);
  aLabelLay->setSpacing(0);

  QLabel* aLbl = new QLabel(aLabelWgt);
  aLbl->setPixmap(QPixmap(":pictures/assembly.png"));
  aLbl->setMargin(2);
  // Do not paint background of the label (in order to show icon)
  aLbl->setAutoFillBackground(false);

  aLabelLay->addWidget(aLbl);

  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->moduleDocument();

  myActiveDocLbl = new XGUI_ActiveDocLbl(tr("Part set"), aLabelWgt);
//  myActiveDocLbl->setReadOnly(true);
//  myActiveDocLbl->setFrame(false);
  myActiveDocLbl->setContextMenuPolicy(Qt::CustomContextMenu);

  aLabelLay->addWidget(myActiveDocLbl);
  aLabelLay->setStretch(1, 1);

  myTreeView = new XGUI_DataTree(this);
  myTreeView->setFrameShape(QFrame::NoFrame);
  aLayout->addWidget(myTreeView);

  QPalette aTreePalet = myTreeView->palette();
  QColor aTreeBack = aTreePalet.color(QPalette::Base);

  QPalette aPalet;
  aPalet.setColor(QPalette::Base, aTreeBack);
  aPalet.setColor(QPalette::Window, aTreeBack);
  aLabelWgt->setPalette(aPalet);

  myDocModel = new XGUI_DataModel(this);
  connect(myDocModel, SIGNAL(beforeTreeRebuild()), SLOT(onBeforeReset()));
  connect(myDocModel, SIGNAL(treeRebuilt()), SLOT(onAfterModelReset()));

  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequested(QContextMenuEvent*)));
}

//***************************************************
XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}

void XGUI_ObjectsBrowser::initialize(ModuleBase_ITreeNode* theRoot)
{
  //myDocModel->setXMLReader(theReader);
  myDocModel->setRoot(theRoot);
  myTreeView->setModel(myDocModel);

  // It has to be done after setting of model
  myActiveDocLbl->setTreeView(myTreeView);

  QItemSelectionModel* aSelMod = myTreeView->selectionModel();
  connect(aSelMod, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

//***************************************************
void XGUI_ObjectsBrowser::onContextMenuRequested(QContextMenuEvent* theEvent)
{
  QModelIndexList aIndexes;
  QObjectPtrList aSelectedData = selectedObjects(&aIndexes);
  bool toEnable = false;

  if (aSelectedData.size() == 1) {
    QModelIndex aSelected = myTreeView->indexAt(theEvent->pos());
    if (!aIndexes.contains(aSelected))
      return; // menu is called on non selected item

    Qt::ItemFlags aFlags = dataModel()->flags(aIndexes.first());
    toEnable = ((aFlags & Qt::ItemIsEditable) != 0);
  }
  foreach(QAction* aCmd, actions()) {
    aCmd->setEnabled(toEnable);
  }
  emit contextMenuRequested(theEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onLabelContextMenuRequested(const QPoint& thePnt)
{
  myTreeView->selectionModel()->clearSelection();
  //Empty feature pointer means that selected root document
  foreach(QAction* aCmd, actions()) {
    aCmd->setEnabled(true);
  }
  QContextMenuEvent aEvent(QContextMenuEvent::Mouse, thePnt, myActiveDocLbl->mapToGlobal(thePnt));
  emit contextMenuRequested(&aEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onEditItem()
{
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
        if (aIdx.column() == 1) {
          ObjectPtr aFea = dataModel()->object(aIdx);
          if (dataModel()->object(aIdx)->isSame(anObject)) {
            aIndex = aIdx;
            break;
          }
        }
      }
      if (aIndex.isValid()) {
        myTreeView->setCurrentIndex(aIndex);
        myTreeView->edit(aIndex);
      }
      return;
    }
  }
}

//***************************************************
QList<ModuleBase_ITreeNode*> XGUI_ObjectsBrowser::expandedItems(const QModelIndex& theParent) const
{
  QList<ModuleBase_ITreeNode*> aIndexes;
  QModelIndex aIndex;
  int aCount = myDocModel->rowCount(theParent);
  for (int i = 0; i < aCount; i++) {
    aIndex = myDocModel->index(i, 0, theParent);
    if (myDocModel->hasChildren(aIndex)) {
      if (myTreeView->isExpanded(aIndex)) {
        aIndexes.append((ModuleBase_ITreeNode*)aIndex.internalPointer());
        QList<ModuleBase_ITreeNode*> aSubIndexes = expandedItems(aIndex);
        if (!aSubIndexes.isEmpty())
          aIndexes.append(aSubIndexes);
      }
    }
  }
  return aIndexes;
}

//***************************************************
void XGUI_ObjectsBrowser::rebuildDataTree()
{
  myDocModel->root()->update();
  myDocModel->rebuildDataTree();
  update();
}

//***************************************************
void XGUI_ObjectsBrowser::setObjectsSelected(const QObjectPtrList& theObjects)
{
  QItemSelectionModel* aSelectModel = myTreeView->selectionModel();
  QModelIndexList aIndexes = aSelectModel->selectedIndexes();
  if (theObjects.size() == 0) {
    bool aIsBlock = aSelectModel->blockSignals(true);
    aSelectModel->clear();
    aSelectModel->blockSignals(aIsBlock);
    foreach(QModelIndex aIdx, aIndexes) {
      myTreeView->update(aIdx);
    }
    return;
  }

  ObjectPtr aObject;
  QModelIndexList aUnselect;
  QObjectPtrList aToSelect = theObjects;
  QHash<qint64, ObjectPtr> aNotChanged;
  foreach(QModelIndex aIdx, aIndexes) {
    aObject = myDocModel->object(aIdx);
    if (aObject.get()) {
      if (aToSelect.contains(aObject)) {
        aNotChanged.insert((qint64)aObject.get(), aObject);
      } else {
        aUnselect.append(aIdx);
      }
    }
    else {
      aUnselect.append(aIdx);
    }
  }

  foreach(ObjectPtr aObj, aNotChanged)
    aToSelect.removeAll(aObj);

  bool aIsBlock = aSelectModel->blockSignals(true);
  foreach(QModelIndex aIdx, aUnselect) {
    aSelectModel->select(aIdx, QItemSelectionModel::Deselect);
    myTreeView->update(aIdx);
  }

  QModelIndex aIndex0, aIndex1, aIndex2, aCurrent;
  foreach(ObjectPtr aFeature, aToSelect) {
    aIndex1 = myDocModel->objectIndex(aFeature, 1);
    if (aIndex1.isValid()) {
      if (!aCurrent.isValid())
        aCurrent = aIndex1;
      aIndex0 = myDocModel->objectIndex(aFeature, 0);
      aIndex2 = myDocModel->objectIndex(aFeature, 2);
      aSelectModel->select(aIndex1, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      myTreeView->update(aIndex0);
      myTreeView->update(aIndex1);
      myTreeView->update(aIndex2);
    }
  }
  aSelectModel->setCurrentIndex(aCurrent, QItemSelectionModel::NoUpdate);
  aSelectModel->blockSignals(aIsBlock);
}

//***************************************************
void XGUI_ObjectsBrowser::ensureVisible(const ObjectPtr theObject)
{
  QModelIndex aIndex = myDocModel->objectIndex(theObject);
  if (aIndex.isValid())  {
    QModelIndex aParent = aIndex.parent();
    while (aParent.isValid()) {
      myTreeView->expand(aParent);
      aParent = aParent.parent();
    }
    myTreeView->scrollTo(aIndex);
  }
}

//***************************************************
void XGUI_ObjectsBrowser::clearContent()
{
  myTreeView->clear();
}

//***************************************************
void XGUI_ObjectsBrowser::onSelectionChanged(const QItemSelection& theSelected,
                                       const QItemSelection& theDeselected)
{
  onSelectionChanged();
}

//***************************************************
void XGUI_ObjectsBrowser::onSelectionChanged()
{
  emit selectionChanged();
}

//***************************************************
QObjectPtrList XGUI_ObjectsBrowser::selectedObjects(QModelIndexList* theIndexes) const
{
  QObjectPtrList aList;
  QModelIndexList aIndexes = selectedIndexes();
  XGUI_DataModel* aModel = dataModel();

  foreach(QModelIndex aIdx, aIndexes) {
    if (aIdx.column() == 1) {
      ObjectPtr aObject = aModel->object(aIdx);
      if (aObject) {
        aList.append(aObject);
        if (theIndexes)
          theIndexes->append(aIdx);
      }
    }
  }
  return aList;
}

void XGUI_ObjectsBrowser::onBeforeReset()
{
  myExpandedItems = expandedItems();
}

void XGUI_ObjectsBrowser::onAfterModelReset()
{
  XGUI_DataModel* aModel = myTreeView->dataModel();
  QModelIndex aIndex;
  foreach(ModuleBase_ITreeNode* aNode, myExpandedItems) {
    if (aModel->hasNode(aNode)) {
      aIndex = aModel->getIndex(aNode, 0);
      if (aIndex.isValid() && (myTreeView->dataModel()->hasIndex(aIndex)))
        myTreeView->setExpanded(aIndex, true);
    }
  }
  myExpandedItems.clear();
}

std::list<bool> XGUI_ObjectsBrowser::getStateForDoc(DocumentPtr theDoc) const
{
  std::list<bool> aStates;
  XGUI_DataModel* aModel = dataModel();
  QModelIndex aRootIdx = aModel->documentRootIndex(theDoc);
  int aNbChild = aModel->rowCount(aRootIdx);
  for (int i = 0; i < aNbChild; i++) {
    QModelIndex aIdx = aModel->index(i, 0, aRootIdx);
    aStates.push_back(myTreeView->isExpanded(aIdx));
  }
  return aStates;
}

void XGUI_ObjectsBrowser::setStateForDoc(DocumentPtr theDoc, const std::list<bool>& theStates)
{
  if (theStates.size() == 0)
    return;
  XGUI_DataModel* aModel = dataModel();
  QModelIndex aRootIdx = aModel->documentRootIndex(theDoc);
  int aNbChild = aModel->rowCount(aRootIdx);

  std::list<bool>::const_iterator aIt;
  int i = 0;
  for (aIt = theStates.cbegin(); aIt != theStates.cend(); aIt++, i++) {
    if (i >= aNbChild )
      break;
    QModelIndex aIdx = aModel->index(i, 0, aRootIdx);
    myTreeView->setExpanded(aIdx, (*aIt));
  }
}

void XGUI_ObjectsBrowser::updateAllIndexes(int theColumn, const QModelIndex& theParent)
{
  int aNb = myDocModel->rowCount(theParent);
  for (int i = 0; i < aNb; i++) {
    QModelIndex aIdx = myDocModel->index(i, theColumn, theParent);
    if (aIdx.isValid() && myDocModel->hasIndex(aIdx)) {
      myTreeView->update(aIdx);
      if (myTreeView->isExpanded(aIdx))
        updateAllIndexes(theColumn, aIdx);
    }
  }
}

QMap<ObjectPtr, bool> XGUI_ObjectsBrowser::getFoldersState(DocumentPtr theDoc) const
{
  QMap<ObjectPtr, bool> aMap;

  int aNb = theDoc->size(ModelAPI_Folder::group());
  ObjectPtr aObj;
  for (int i = 0; i < aNb; i++) {
    aObj = theDoc->object(ModelAPI_Folder::group(), i);
    QModelIndex aIdx = myDocModel->objectIndex(aObj, 0);
    aMap[aObj] = myTreeView->isExpanded(aIdx);
  }
  return aMap;
}

void XGUI_ObjectsBrowser::setFoldersState(const QMap<ObjectPtr, bool>& theStates)
{
  QMap<ObjectPtr, bool>::const_iterator aIt;
  for (aIt = theStates.constBegin(); aIt != theStates.constEnd(); aIt++) {
    QModelIndex aIdx = myDocModel->objectIndex(aIt.key(), 0);
    myTreeView->setExpanded(aIdx, aIt.value());
  }
}


void XGUI_ObjectsBrowser::resizeEvent(QResizeEvent* theEvent)
{
  QWidget::resizeEvent(theEvent);
  emit sizeChanged();
}

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Tools.h"
#include "XGUI_DataModel.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_Tools.h>

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>
#include <QMessageBox>

/// Width of second column (minimum acceptable = 27)
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
  XGUI_TreeViewItemDelegate(XGUI_DataTree* theParent):QStyledItemDelegate(theParent), myTreedView(theParent) {}

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
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  setItemDelegateForColumn(0, new XGUI_TreeViewItemDelegate(this));

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
  QLineEdit* aEditor = dynamic_cast<QLineEdit*>(theEditor);
  if (aEditor) {
    QString aName = aEditor->text();
    QModelIndexList aIndexList = selectionModel()->selectedIndexes();
    XGUI_DataModel* aModel = dataModel();
    ObjectPtr aObj = aModel->object(aIndexList.first());
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation("Rename");

    if (!XGUI_Tools::canRename(this, aObj, aName)) {
      aMgr->abortOperation();
      return;
    }

    aObj->data()->setName(qPrintable(aName));
    aMgr->finishOperation();
  }
}

void XGUI_DataTree::clear() 
{
  dataModel()->clear();
  reset();
}

void XGUI_DataTree::resizeEvent(QResizeEvent* theEvent)
{
  QSize aSize = theEvent->size();
  if (aSize.isValid()) {
    setColumnWidth(0, aSize.width() - SECOND_COL_WIDTH);
    setColumnWidth(1, SECOND_COL_WIDTH);
  }
}

void XGUI_DataTree::onDoubleClick(const QModelIndex& theIndex)
{
  if (theIndex.column() != 1)
    return;
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
    if ((theIndex.internalId() == -1) && (aDoc != aMgr->moduleDocument()))
      // Clicked folder under root but active document is another
      return;
    if ((theIndex.internalId() != -1) && (aDoc.get() != theIndex.internalPointer()))
      // Cliced not on active document folder
      return;

    aMgr->startOperation(aOpName);
    aDoc->setCurrentFeature(FeaturePtr(), true);
    aMgr->finishOperation();
  }
  QModelIndex aNewIndex = aModel->lastHistoryIndex();
  QModelIndex aParent = theIndex.parent();
  int aSize = aModel->rowCount(aParent);
  for (int i = 0; i < aSize; i++) {
    update(aModel->index(i, 0, aParent));
  }
}

//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
    : QWidget(theParent), myDocModel(0)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->setSpacing(0);

  QFrame* aLabelWgt = new QFrame(this);
  aLabelWgt->setAutoFillBackground(true);
  QPalette aPalet = aLabelWgt->palette();
  aPalet.setColor(QPalette::Window, Qt::white);
  aLabelWgt->setPalette(aPalet);

  aLayout->addWidget(aLabelWgt);
  QHBoxLayout* aLabelLay = new QHBoxLayout(aLabelWgt);
  ModuleBase_Tools::zeroMargins(aLabelLay);
  aLabelLay->setSpacing(0);

  QLabel* aLbl = new QLabel(aLabelWgt);
  aLbl->setPixmap(QPixmap(":pictures/assembly.png"));
  aLbl->setMargin(2);

  aLbl->setAutoFillBackground(true);

  aLabelLay->addWidget(aLbl);

  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->moduleDocument();
  // TODO: Find a name of the root document

  myActiveDocLbl = new QLineEdit(tr("Part set"), aLabelWgt);
  myActiveDocLbl->setReadOnly(true);
  myActiveDocLbl->setFrame(false);
  //myActiveDocLbl->setMargin(2);
  myActiveDocLbl->setContextMenuPolicy(Qt::CustomContextMenu);

  myActiveDocLbl->installEventFilter(this);

  aLabelLay->addWidget(myActiveDocLbl);
  aLabelLay->setStretch(1, 1);

  myTreeView = new XGUI_DataTree(this);
  aLayout->addWidget(myTreeView);

  aLabelWgt->setFrameShape(myTreeView->frameShape());
  aLabelWgt->setFrameShadow(myTreeView->frameShadow());

  myDocModel = new XGUI_DataModel(this);
  myTreeView->setModel(myDocModel);
  QItemSelectionModel* aSelMod = myTreeView->selectionModel();
  connect(aSelMod, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));

  connect(myActiveDocLbl, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(onLabelContextMenuRequested(const QPoint&)));
  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequested(QContextMenuEvent*)));
}

//***************************************************
XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}

//***************************************************
bool XGUI_ObjectsBrowser::eventFilter(QObject* obj, QEvent* theEvent)
{
  if (obj == myActiveDocLbl) {
    if (!myActiveDocLbl->isReadOnly()) {
      // End of editing by mouse click
      if (theEvent->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* aEvent = (QMouseEvent*) theEvent;
        QPoint aPnt = mapFromGlobal(aEvent->globalPos());
        if (childAt(aPnt) != myActiveDocLbl) {
          closeDocNameEditing(true);
        }
      } else if (theEvent->type() == QEvent::KeyRelease) {
        QKeyEvent* aEvent = (QKeyEvent*) theEvent;
        switch (aEvent->key()) {
          case Qt::Key_Return:
          case Qt::Key_Enter:  // Accept current input
            closeDocNameEditing(true);
            break;
          case Qt::Key_Escape:  // Cancel the input
            closeDocNameEditing(false);
            break;
        }
      }
    } else {
      if (theEvent->type() == QEvent::MouseButtonDblClick) {
        emit headerMouseDblClicked(QModelIndex());
        return true;
      }  
    }
  }
  return QWidget::eventFilter(obj, theEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::closeDocNameEditing(bool toSave)
{
  myActiveDocLbl->deselect();
  myActiveDocLbl->clearFocus();
  myActiveDocLbl->releaseMouse();
  myActiveDocLbl->setReadOnly(true);
  if (toSave) {
    // TODO: Save the name of root document
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->moduleDocument();
  } else {
    myActiveDocLbl->setText(myActiveDocLbl->property("OldText").toString());
  }
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
    ObjectPtr aFeature = aSelectedData.first();
    if (aFeature) {  // Selection happens in TreeView
      QObjectPtrList aList;
      aList.append(aFeature);
      // check whether the object can be deleted. There should not be parts which are not loaded
      if (!XGUI_Tools::canRemoveOrRename((QWidget*)parent(), aList))
        return;

      // Find index which corresponds the feature
      QModelIndex aIndex;
      foreach(QModelIndex aIdx, selectedIndexes()) {
        ObjectPtr aFea = dataModel()->object(aIdx);
        if (dataModel()->object(aIdx)->isSame(aFeature)) {
          aIndex = aIdx;
          break;
        }
      }
      if (aIndex.isValid()) {
        myTreeView->setCurrentIndex(aIndex);
        myTreeView->edit(aIndex);
      }
      return;
    }
  }
  //Selection happens in Upper label
  myActiveDocLbl->setReadOnly(false);
  myActiveDocLbl->setFocus();
  myActiveDocLbl->selectAll();
  myActiveDocLbl->grabMouse();
  myActiveDocLbl->setProperty("OldText", myActiveDocLbl->text());
}

//***************************************************
void XGUI_ObjectsBrowser::rebuildDataTree()
{
  myDocModel->rebuildDataTree();
  update();
}

//***************************************************
void XGUI_ObjectsBrowser::setObjectsSelected(const QObjectPtrList& theObjects)
{
  QList<QModelIndex> theIndexes;
  QItemSelectionModel* aSelectModel = myTreeView->selectionModel();
  aSelectModel->clear();

  foreach(ObjectPtr aFeature, theObjects)
  {
    QModelIndex aIndex = myDocModel->objectIndex(aFeature);
    if (aIndex.isValid()) {
      aSelectModel->select(aIndex, QItemSelectionModel::Select);
    }
  }
}

//***************************************************
void XGUI_ObjectsBrowser::clearContent()  
{ 
  myTreeView->clear(); 
}

void XGUI_ObjectsBrowser::onSelectionChanged(const QItemSelection& theSelected,
                                       const QItemSelection& theDeselected)
{
  emit selectionChanged();
}

QObjectPtrList XGUI_ObjectsBrowser::selectedObjects(QModelIndexList* theIndexes) const
{
  QObjectPtrList aList;
  QModelIndexList aIndexes = selectedIndexes();
  XGUI_DataModel* aModel = dataModel();
  QModelIndexList::const_iterator aIt;
  for (aIt = aIndexes.constBegin(); aIt != aIndexes.constEnd(); ++aIt) {
    if ((*aIt).column() == 0) {
      ObjectPtr aObject = aModel->object(*aIt);
      if (aObject) {
        aList.append(aObject);
        if (theIndexes)
          theIndexes->append(*aIt);
      }
    }
  }
  return aList;
}
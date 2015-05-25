// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_IDocumentDataModel.h>

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>


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
      ModuleBase_IDocumentDataModel* aModel = myTreedView->dataModel();
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
}

XGUI_DataTree::~XGUI_DataTree()
{
}

ModuleBase_IDocumentDataModel* XGUI_DataTree::dataModel() const
{
  return static_cast<ModuleBase_IDocumentDataModel*>(model());
}

void XGUI_DataTree::contextMenuEvent(QContextMenuEvent* theEvent)
{
  emit contextMenuRequested(theEvent);
}

void XGUI_DataTree::commitData(QWidget* theEditor)
{
  QLineEdit* aEditor = dynamic_cast<QLineEdit*>(theEditor);
  if (aEditor) {
    QString aRes = aEditor->text();
    QModelIndexList aIndexList = selectionModel()->selectedIndexes();
    ModuleBase_IDocumentDataModel* aModel = dataModel();
    ObjectPtr aObj = aModel->object(aIndexList.first());
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation("Rename");
    aObj->data()->setName(qPrintable(aRes));
    aMgr->finishOperation();
  }
}

void XGUI_DataTree::clear() 
{
  ModuleBase_IDocumentDataModel* aModel = dataModel();
  aModel->clear();
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

  connect(myActiveDocLbl, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(onLabelContextMenuRequested(const QPoint&)));
  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequested(QContextMenuEvent*)));

  // Create internal actions
  QAction* aAction = new QAction(QIcon(":pictures/rename_edit.png"), tr("Rename"), this);
  aAction->setData("RENAME_CMD");
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onEditItem()));
  addAction(aAction);
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

void XGUI_ObjectsBrowser::setDataModel(ModuleBase_IDocumentDataModel* theModel)
{
  myDocModel = theModel;
  myTreeView->setModel(myDocModel);
  QItemSelectionModel* aSelMod = myTreeView->selectionModel();
  connect(aSelMod, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
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
  ModuleBase_IDocumentDataModel* aModel = dataModel();
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
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"
#include "XGUI_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>

#include <ModuleBase_Tools.h>

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>

class XGUI_TreeViewItemDelegate: public QStyledItemDelegate
{
public:
  XGUI_TreeViewItemDelegate(XGUI_DataTree* theParent):QStyledItemDelegate(theParent), myTreedView(theParent) {}

  virtual void	setEditorData ( QWidget* editor, const QModelIndex& index ) const
  {
    QLineEdit* aEditor = dynamic_cast<QLineEdit*>(editor);
    if (aEditor) {
      XGUI_DocumentDataModel* aModel = myTreedView->dataModel();
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
  setModel(new XGUI_DocumentDataModel(this));
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  setItemDelegateForColumn(0, new XGUI_TreeViewItemDelegate(this));

  connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

XGUI_DataTree::~XGUI_DataTree()
{
}

XGUI_DocumentDataModel* XGUI_DataTree::dataModel() const
{
  return static_cast<XGUI_DocumentDataModel*>(model());
}

void XGUI_DataTree::onSelectionChanged(const QItemSelection& theSelected,
                                       const QItemSelection& theDeselected)
{
  mySelectedData.clear();
  QModelIndexList aIndexes = selectionModel()->selectedIndexes();
  XGUI_DocumentDataModel* aModel = dataModel();
  QModelIndexList::const_iterator aIt;
  for (aIt = aIndexes.constBegin(); aIt != aIndexes.constEnd(); ++aIt) {
    ObjectPtr aObject = aModel->object(*aIt);
    if (aObject)
      mySelectedData.append(aObject);
  }
  emit selectionChanged();
}

void XGUI_DataTree::mouseDoubleClickEvent(QMouseEvent* theEvent)
{
  if (theEvent->button() == Qt::LeftButton) {
    QModelIndex aIndex = currentIndex();
    XGUI_DocumentDataModel* aModel = dataModel();
    ObjectPtr aObject = aModel->object(aIndex);
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    if (aPart) {
      aPart->activate();
    }
  } else
    QTreeView::mouseDoubleClickEvent(theEvent);
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
    ObjectPtr aFeature = mySelectedData.first();
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation("Rename");
    aFeature->data()->setName(qPrintable(aRes));
    aMgr->finishOperation();
  }
}

void XGUI_DataTree::clear() 
{
  mySelectedData.clear();
  XGUI_DocumentDataModel* aModel = dataModel();
  aModel->clear();
  reset();
}

//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
    : QWidget(theParent)
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

  myDocModel = myTreeView->dataModel();

  aLabelWgt->setFrameShape(myTreeView->frameShape());
  aLabelWgt->setFrameShadow(myTreeView->frameShadow());

  connect(myTreeView, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  connect(myTreeView, SIGNAL(activePartChanged(ObjectPtr)), this,
          SLOT(onActivePartChanged(ObjectPtr)));
  connect(myTreeView, SIGNAL(activePartChanged(ObjectPtr)), this,
          SIGNAL(activePartChanged(ObjectPtr)));

  connect(myActiveDocLbl, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(onLabelContextMenuRequested(const QPoint&)));
  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequested(QContextMenuEvent*)));

  onActivePartChanged(ObjectPtr());

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
void XGUI_ObjectsBrowser::onActivePartChanged(ObjectPtr thePart)
{
  QPalette aPalet = myActiveDocLbl->palette();
  if (thePart) {
    aPalet.setColor(QPalette::Text, Qt::black);
  } else {
    aPalet.setColor(QPalette::Text, QColor(0, 72, 140));
  }
  myActiveDocLbl->setPalette(aPalet);
}

//***************************************************
bool XGUI_ObjectsBrowser::eventFilter(QObject* obj, QEvent* theEvent)
{
  if (obj == myActiveDocLbl) {
    if (myActiveDocLbl->isReadOnly()) {
      if (theEvent->type() == QEvent::MouseButtonDblClick) {
        if (myDocModel->activePartIndex().isValid()) {
          myTreeView->setExpanded(myDocModel->activePartIndex(), false);
        }
        myDocModel->deactivatePart();
        onActivePartChanged(ObjectPtr());
        emit activePartChanged(ObjectPtr());
      }
    } else {
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
void XGUI_ObjectsBrowser::activatePart(const ResultPartPtr& thePart)
{
  if (thePart) {
    QModelIndex aIndex = myDocModel->partIndex(thePart);

    if ((myDocModel->activePartIndex() != aIndex) && myDocModel->activePartIndex().isValid()) {
      myTreeView->setExpanded(myDocModel->activePartIndex(), false);
    }
    bool isChanged = myDocModel->activatedIndex(aIndex);
    if (isChanged) {
      if (myDocModel->activePartIndex().isValid()) {
        myTreeView->setExpanded(aIndex.parent(), true);
        myTreeView->setExpanded(aIndex, true);
        onActivePartChanged(myDocModel->object(aIndex));
      } else {
        onActivePartChanged(ObjectPtr());
      }
    }
  } else {
    QModelIndex aIndex = myDocModel->activePartIndex();
    if (aIndex.isValid()) {
      myDocModel->activatedIndex(aIndex);
      myTreeView->setExpanded(aIndex, false);
      onActivePartChanged(ObjectPtr());
    }
  }
}

//***************************************************
void XGUI_ObjectsBrowser::onContextMenuRequested(QContextMenuEvent* theEvent)
{
  myObjectsList = myTreeView->selectedObjects();
  bool toEnable = myObjectsList.size() == 1;
  foreach(QAction* aCmd, actions())
  {
    aCmd->setEnabled(toEnable);
  }
  emit contextMenuRequested(theEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onLabelContextMenuRequested(const QPoint& thePnt)
{
  myObjectsList.clear();
  //Empty feature pointer means that selected root document
  myObjectsList.append(ObjectPtr());

  foreach(QAction* aCmd, actions())
  {
    aCmd->setEnabled(true);
  }
  QContextMenuEvent aEvent(QContextMenuEvent::Mouse, thePnt, myActiveDocLbl->mapToGlobal(thePnt));
  emit contextMenuRequested(&aEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onEditItem()
{
  if (myObjectsList.size() > 0) {
    ObjectPtr aFeature = myObjectsList.first();
    if (aFeature) {  // Selection happens in TreeView
      // Find index which corresponds the feature
      QModelIndex aIndex;
      foreach(QModelIndex aIdx, selectedIndexes())
      {
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
    } else {  //Selection happens in Upper label
      myActiveDocLbl->setReadOnly(false);
      myActiveDocLbl->setFocus();
      myActiveDocLbl->selectAll();
      myActiveDocLbl->grabMouse();
      myActiveDocLbl->setProperty("OldText", myActiveDocLbl->text());
    }
  }
}

//***************************************************
void XGUI_ObjectsBrowser::onSelectionChanged()
{
  myObjectsList = myTreeView->selectedObjects();
  emit selectionChanged();
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
void XGUI_ObjectsBrowser::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{ 
  myDocModel->processEvent(theMessage); 
}


//***************************************************
void XGUI_ObjectsBrowser::clearContent()  
{ 
  myObjectsList.clear();
  myTreeView->clear(); 
}

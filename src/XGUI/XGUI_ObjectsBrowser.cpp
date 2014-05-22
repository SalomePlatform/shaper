#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>



XGUI_DataTree::XGUI_DataTree(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  setModel(new XGUI_DocumentDataModel(this));
  setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    FeaturePtr aFeature = aModel->feature(*aIt);
    if (aFeature)
      mySelectedData.append(aFeature);
  }
  emit selectionChanged();
}

void XGUI_DataTree::mouseDoubleClickEvent(QMouseEvent* theEvent)
{
  if (theEvent->button() == Qt::LeftButton) {
    QModelIndex aIndex = currentIndex();
    XGUI_DocumentDataModel* aModel = dataModel();

    if ((aModel->activePartIndex() != aIndex) && aModel->activePartIndex().isValid()) {
      setExpanded(aModel->activePartIndex(), false);
    }
    bool isChanged = aModel->activatedIndex(aIndex);
    QTreeView::mouseDoubleClickEvent(theEvent);
    if (isChanged) {
      if (aModel->activePartIndex().isValid())
        setExpanded(aIndex, true);
      emit activePartChanged(aModel->activePart());
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
    FeaturePtr aFeature = mySelectedData.first();
    aFeature->data()->setName(qPrintable(aRes));
  }
}

//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QWidget(theParent)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(0);

  QFrame* aLabelWgt = new QFrame(this);
  aLabelWgt->setAutoFillBackground(true);
  QPalette aPalet = aLabelWgt->palette();
  aPalet.setColor(QPalette::Window, Qt::white);
  aLabelWgt->setPalette(aPalet);

  aLayout->addWidget(aLabelWgt);
  QHBoxLayout* aLabelLay = new QHBoxLayout(aLabelWgt);
  aLabelLay->setContentsMargins(0, 0, 0, 0);
  aLabelLay->setSpacing(0);

  QLabel* aLbl = new QLabel(aLabelWgt);
  aLbl->setPixmap(QPixmap(":pictures/assembly.png"));
  aLbl->setMargin(2);

  aLbl->setAutoFillBackground(true);

  aLabelLay->addWidget(aLbl);

  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  // TODO: Find a name of the root document

  myActiveDocLbl = new QLineEdit(tr("Part set"), aLabelWgt);
  myActiveDocLbl->setReadOnly(true);
  myActiveDocLbl->setFrame(false);
  //myActiveDocLbl->setMargin(2);
  myActiveDocLbl->setContextMenuPolicy(Qt::CustomContextMenu);

  myActiveDocLbl->installEventFilter(this);

  aLabelLay->addWidget(myActiveDocLbl);
  aLabelLay->setStretch(1,1);

  myTreeView = new XGUI_DataTree(this);
  aLayout->addWidget(myTreeView);

  myDocModel = myTreeView->dataModel();

  aLabelWgt->setFrameShape(myTreeView->frameShape());
  aLabelWgt->setFrameShadow(myTreeView->frameShadow());

  connect(myTreeView, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  connect(myTreeView, SIGNAL(activePartChanged(FeaturePtr)), this, SLOT(onActivePartChanged(FeaturePtr)));
  connect(myTreeView, SIGNAL(activePartChanged(FeaturePtr)), this, SIGNAL(activePartChanged(FeaturePtr)));

  connect(myActiveDocLbl, SIGNAL(customContextMenuRequested(const QPoint&)), 
          this, SLOT(onLabelContextMenuRequested(const QPoint&)));
  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), 
          this, SLOT(onContextMenuRequested(QContextMenuEvent*)));
  
  onActivePartChanged(FeaturePtr());

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
void XGUI_ObjectsBrowser::onActivePartChanged(FeaturePtr thePart)
{
  QPalette aPalet = myActiveDocLbl->palette();
  if (thePart) {
    aPalet.setColor(QPalette::Text, Qt::black);
  }  else {
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
        onActivePartChanged(FeaturePtr());
        emit activePartChanged(FeaturePtr());
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
        case Qt::Key_Return: // Accept current input
          closeDocNameEditing(true);
          break;
        case Qt::Key_Escape: // Cancel the input
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
    PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
    DocumentPtr aDoc = aMgr->rootDocument();
  } else {
    myActiveDocLbl->setText(myActiveDocLbl->property("OldText").toString());
  }
}

//***************************************************
void XGUI_ObjectsBrowser::activatePart(const FeaturePtr& thePart)
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
        onActivePartChanged(myDocModel->feature(aIndex));
      } else {
        onActivePartChanged(FeaturePtr());
      }
    }
  } else {
    QModelIndex aIndex = myDocModel->activePartIndex();
    if (aIndex.isValid()) {
      myDocModel->activatedIndex(aIndex);
      myTreeView->setExpanded(aIndex, false);
      onActivePartChanged(FeaturePtr());
    }
  }
}

//***************************************************
void XGUI_ObjectsBrowser::onContextMenuRequested(QContextMenuEvent* theEvent) 
{
  myFeaturesList = myTreeView->selectedFeatures();
  bool toEnable = myFeaturesList.size() > 0;
  foreach(QAction* aCmd, actions()) {
    aCmd->setEnabled(toEnable);
  }
  emit contextMenuRequested(theEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onLabelContextMenuRequested(const QPoint& thePnt)
{
  myFeaturesList.clear();
  //Empty feature pointer means that selected root document
  myFeaturesList.append(FeaturePtr()); 

  foreach(QAction* aCmd, actions()) {
    aCmd->setEnabled(true);
  }
  QContextMenuEvent aEvent( QContextMenuEvent::Mouse, thePnt, myActiveDocLbl->mapToGlobal(thePnt) );
  emit contextMenuRequested(&aEvent);
}

//***************************************************
void XGUI_ObjectsBrowser::onEditItem()
{
  if (myFeaturesList.size() > 0) {
    FeaturePtr aFeature = myFeaturesList.first();
    if (aFeature) { // Selection happens in TreeView
      // Find index which corresponds the feature
      QModelIndex aIndex;
      foreach(QModelIndex aIdx, selectedIndexes()) {
        if (dataModel()->feature(aIdx) == aFeature) {
          aIndex = aIdx;
          break;
        }
      }
      if (aIndex.isValid()) {
        myTreeView->setCurrentIndex(aIndex);
        myTreeView->edit(aIndex);
      }
    } else { //Selection happens in Upper label
      myActiveDocLbl->setReadOnly(false);
      myActiveDocLbl->setFocus();
      myActiveDocLbl->selectAll();
      myActiveDocLbl->grabMouse();
      myActiveDocLbl->setProperty("OldText", myActiveDocLbl->text());
    }
  }
}
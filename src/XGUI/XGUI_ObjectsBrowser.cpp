// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Tools.h"
#include "XGUI_DataModel.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Tools.h>
#include <Events_Error.h>

#include <ModuleBase_Tools.h>

#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QAction>
#include <QStyledItemDelegate>
#include <QMessageBox>

#ifdef WIN32
#ifdef HAVE_SALOME
#include <QWindowsStyle>
#endif
#endif


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
#ifdef WIN32
#ifdef HAVE_SALOME
  setStyle(new QWindowsStyle());
#else
  myStyle = new XGUI_TreeViewStyle();
  setStyle(myStyle);
#endif
#endif

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
  static int aEntrance = 0;
  if (aEntrance == 0) {
    // We have to check number of enter and exit of this function because it can be called recursively by Qt
    // in order to avoid double modifying of a data
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

#if (!defined HAVE_SALOME) && (defined WIN32)
void XGUI_DataTree::drawRow(QPainter* thePainter,
                            const QStyleOptionViewItem& theOptions,
                            const QModelIndex& theIndex) const
{
  QStyleOptionViewItemV4 aOptions = theOptions;
  myStyle->setIndex(theIndex);
  QTreeView::drawRow(thePainter, aOptions, theIndex);
}

//********************************************************************
//********************************************************************
//********************************************************************
void XGUI_TreeViewStyle::drawPrimitive(PrimitiveElement theElement, 
                                       const QStyleOption* theOption,
                                       QPainter* thePainter, const QWidget* theWidget) const
{
  if ((theElement == QStyle::PE_PanelItemViewRow) || (theElement == QStyle::PE_PanelItemViewItem)) {
    const QStyleOptionViewItemV4* aOptions = qstyleoption_cast<const QStyleOptionViewItemV4 *>(theOption);
    if (myIndex.isValid() && ((myIndex.flags() & Qt::ItemIsSelectable) == 0)) {
      QStyle::State aState = aOptions->state;
      if ((aState & QStyle::State_MouseOver) != 0)
        aState &= ~QStyle::State_MouseOver;
      QStyleOptionViewItemV4* aOpt = (QStyleOptionViewItemV4*) aOptions;
      aOpt->state = aState;
      QWindowsVistaStyle::drawPrimitive(theElement, aOpt, thePainter, theWidget);
    }
  }
  QWindowsVistaStyle::drawPrimitive(theElement, theOption, thePainter, theWidget);
}
#endif


//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ActiveDocLbl::XGUI_ActiveDocLbl(const QString& theText, QWidget* theParent )
  : QLineEdit(theText, theParent), 
  myPreSelectionStyle(""), 
  myNeutralStyle(""), 
  mySelectionStyle(""),
  myIsSelected(false)
{
}

void XGUI_ActiveDocLbl::setTreeView(QTreeView* theView)
{
  myTreeView = theView;
  QPalette aPalet = myTreeView->palette();
  QColor aHighlight = aPalet.highlight().color();
  QColor aHighlightText = aPalet.highlightedText().color();

  myPreSelectionStyle = "QLineEdit {background-color: ";
  myPreSelectionStyle += "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 " + aHighlight.lighter(170).name() + ");"; 
  myPreSelectionStyle += "border: 1px solid lightblue; border-radius: 2px }";

  QString aName = aPalet.color(QPalette::Base).name();
  myNeutralStyle = "QLineEdit { border: 1px solid " + aName + " }";


#if (!defined HAVE_SALOME) && (defined WIN32)
  mySelectionStyle = "QLineEdit {background-color: ";
  mySelectionStyle += "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(236, 245, 255)";
  mySelectionStyle += ", stop:1 rgb(208, 229, 255));"; 
  mySelectionStyle += "border: 1px solid rgb(132, 172, 221); border-radius: 2px }";
#else
  mySelectionStyle = "QLineEdit {background-color: " + aHighlight.name();
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
  return QLineEdit::event(theEvent);
}
#endif

bool XGUI_ActiveDocLbl::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myTreeView->viewport()) {
    if (theEvent->type() == QEvent::MouseButtonRelease)
      unselect();
  }
  return QLineEdit::eventFilter(theObj, theEvent);
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
  QLineEdit::mouseReleaseEvent(e);
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
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
    : QWidget(theParent), myDocModel(0)
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
  myActiveDocLbl->setReadOnly(true);
  myActiveDocLbl->setFrame(false);
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
  myTreeView->setModel(myDocModel);

  // It has to be done after setting of model
  myActiveDocLbl->setTreeView(myTreeView);

  QItemSelectionModel* aSelMod = myTreeView->selectionModel();
  connect(aSelMod, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));

  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequested(QContextMenuEvent*)));
}

//***************************************************
XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
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
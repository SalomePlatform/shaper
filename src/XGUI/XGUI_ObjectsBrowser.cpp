#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"

#include <ModelAPI_Data.h>

#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>



XGUI_DataTree::XGUI_DataTree(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  setModel(new XGUI_DocumentDataModel(this));

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

//********************************************************************
//********************************************************************
//********************************************************************
XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QWidget(theParent)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(0);

  QWidget* aLabelWgt = new QWidget(this);
  aLayout->addWidget(aLabelWgt);
  QHBoxLayout* aLabelLay = new QHBoxLayout(aLabelWgt);
  aLabelLay->setContentsMargins(3, 3, 3, 3);

  QLabel* aLbl = new QLabel(aLabelWgt);
  aLbl->setPixmap(QPixmap(":pictures/assembly.png"));
  aLabelLay->addWidget(aLbl);

  myActiveDocLbl = new QLabel("", aLabelWgt);
  myActiveDocLbl->setAlignment(Qt::AlignHCenter);

  QFont aFnt = myActiveDocLbl->font();
  aFnt.setBold(true);
  myActiveDocLbl->setFont(aFnt);

  myActiveDocLbl->installEventFilter(this);

  aLabelLay->addWidget(myActiveDocLbl);
  aLabelLay->setStretch(1,1);

  myTreeView = new XGUI_DataTree(this);
  aLayout->addWidget(myTreeView);

  myDocModel = myTreeView->dataModel();

  connect(myTreeView, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  connect(myTreeView, SIGNAL(activePartChanged(FeaturePtr)), this, SLOT(onActivePartChanged(FeaturePtr)));
  connect(myTreeView, SIGNAL(activePartChanged(FeaturePtr)), this, SIGNAL(activePartChanged(FeaturePtr)));
  connect(myTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*)), 
          this, SIGNAL(contextMenuRequested(QContextMenuEvent*)));
  
  onActivePartChanged(FeaturePtr());
}


XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}


void XGUI_ObjectsBrowser::onActivePartChanged(FeaturePtr thePart)
{
  QPalette aPalet = myActiveDocLbl->palette();
  if (thePart) {
    myActiveDocLbl->setText(tr("Activate Part set"));
    aPalet.setColor(QPalette::Foreground, Qt::black);
    myActiveDocLbl->setCursor(Qt::PointingHandCursor);
  }  else {
    myActiveDocLbl->setText(tr("Part set is active"));
    aPalet.setColor(QPalette::Foreground, QColor(0, 72, 140));
    myActiveDocLbl->unsetCursor();
  }
  myActiveDocLbl->setPalette(aPalet);
}

bool XGUI_ObjectsBrowser::eventFilter(QObject* obj, QEvent* theEvent)
{
  if (obj == myActiveDocLbl) {
    if (theEvent->type() == QEvent::MouseButtonDblClick) {
      if (myDocModel->activePartIndex().isValid()) {
        myTreeView->setExpanded(myDocModel->activePartIndex(), false);
      }
      myDocModel->deactivatePart();
      onActivePartChanged(FeaturePtr());
      emit activePartChanged(FeaturePtr());
    }
  }
  return QWidget::eventFilter(obj, theEvent);
}

void XGUI_ObjectsBrowser::activateCurrentPart(bool toActivate)
{
  if (toActivate) {
    QModelIndex aIndex = myTreeView->currentIndex();

    if ((myDocModel->activePartIndex() != aIndex) && myDocModel->activePartIndex().isValid()) {
      myTreeView->setExpanded(myDocModel->activePartIndex(), false);
    }
    bool isChanged = myDocModel->activatedIndex(aIndex);
    if ((isChanged) && (myDocModel->activePartIndex().isValid())) {
      myTreeView->setExpanded(aIndex, true);
      onActivePartChanged(myDocModel->feature(aIndex));
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
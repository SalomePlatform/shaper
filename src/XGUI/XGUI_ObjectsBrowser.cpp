#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"

XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  myDocModel = new XGUI_DocumentDataModel(this);
  setModel(myDocModel);

  connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
    this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
}


XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}



void XGUI_ObjectsBrowser::onSelectionChanged(const QItemSelection& theSelected, 
                                             const QItemSelection& theDeselected)
{
  mySelectedData.clear();
  QModelIndexList aIndexes = selectionModel()->selectedIndexes();
  QModelIndexList::const_iterator aIt;
  for (aIt = aIndexes.constBegin(); aIt != aIndexes.constEnd(); ++aIt) {
    FeaturePtr aFeature = myDocModel->feature(*aIt);
    if (aFeature)
      mySelectedData.append(aFeature);
  }
  emit selectionChanged();
}
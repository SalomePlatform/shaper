#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"

XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  XGUI_DocumentDataModel* aDocModel = new XGUI_DocumentDataModel(this);
  setModel(aDocModel);
}


XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}


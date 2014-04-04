#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"


XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  myDocModel = new XGUI_DocumentDataModel(this);
  setModel(myDocModel);
}


XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}


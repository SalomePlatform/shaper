#include "XGUI_ObjectsBrowser.h"
#include "XGUI_DocumentDataModel.h"
//#include "XGUI_PartDataModel.h"

//#include <ModelAPI_PluginManager.h>
//#include <ModelAPI_Iterator.h>
//#include <ModelAPI_Document.h>
//#include <ModelAPI_Feature.h>
//#include <ModelAPI_Object.h>
//#include <Model_Document.h>

XGUI_ObjectsBrowser::XGUI_ObjectsBrowser(QWidget* theParent)
  : QTreeView(theParent)
{
  setHeaderHidden(true);
  XGUI_DocumentDataModel* aDocModel = new XGUI_DocumentDataModel(this);
  //std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  //std::shared_ptr<ModelAPI_Document> aDocument = aMgr->currentDocument();
  //aDocModel->setDocument(aDocument, 0);
  setModel(aDocModel);
}


XGUI_ObjectsBrowser::~XGUI_ObjectsBrowser()
{
}


#include "XGUI_SelectionMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>



XGUI_SelectionMgr::XGUI_SelectionMgr(XGUI_Workshop* theParent) :
  QObject(theParent), myWorkshop(theParent)
{
  XGUI_ObjectsBrowser* aObjBrowser = myWorkshop->mainWindow()->objectBrowser();

  connect(aObjBrowser, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}


XGUI_SelectionMgr::~XGUI_SelectionMgr()
{
}

void XGUI_SelectionMgr::onSelectionChanged()
{
  XGUI_ObjectsBrowser* aObjBrowser = myWorkshop->mainWindow()->objectBrowser();
  mySelectedData = aObjBrowser->selectedData();
  
  // Set current document
  if (mySelectedData.size() > 0) {
    FeaturePtr aFeature = mySelectedData.first();

    std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
    aMgr->setCurrentDocument(aFeature->data()->docRef("PartDocument")->value());
  }

  emit selectionChanged();
}
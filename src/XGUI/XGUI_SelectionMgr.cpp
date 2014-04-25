#include "XGUI_SelectionMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>



XGUI_SelectionMgr::XGUI_SelectionMgr(XGUI_Workshop* theParent) :
  QObject(theParent), myWorkshop(theParent)
{
}

void XGUI_SelectionMgr::connectObjectBrowser(XGUI_ObjectsBrowser* theOB)
{
  myObjectBrowser = theOB;
  connect(myObjectBrowser, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

XGUI_SelectionMgr::~XGUI_SelectionMgr()
{
}

void XGUI_SelectionMgr::onSelectionChanged()
{
  XGUI_ObjectsBrowser* aObjBrowser = myWorkshop->objectBrowser();
  mySelectedData = aObjBrowser->selectedData();
  
  // Set current document
  if (mySelectedData.size() > 0) {
    FeaturePtr aFeature = mySelectedData.first();

    boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
    boost::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = aFeature->data()->docRef("PartDocument");
    if (aDocRef)
      aMgr->setCurrentDocument(aDocRef->value());
  }

  emit selectionChanged();
}
#include "XGUI_SelectionMgr.h"

#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Displayer.h"
#include "XGUI_Selection.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Object.h>



XGUI_SelectionMgr::XGUI_SelectionMgr(XGUI_Workshop* theParent) :
  QObject(theParent), myWorkshop(theParent)
{
  mySelection = new XGUI_Selection(myWorkshop);
}

XGUI_SelectionMgr::~XGUI_SelectionMgr()
{
  delete mySelection;
}

//**************************************************************
void XGUI_SelectionMgr::connectViewers()
{
  connect(myWorkshop->objectBrowser(), SIGNAL(selectionChanged()), 
    this, SLOT(onObjectBrowserSelection()));

  //Connect to other viewers
  connect(myWorkshop->viewer(), SIGNAL(selectionChanged()),
    this, SLOT(onViewerSelection()));
}

//**************************************************************
void XGUI_SelectionMgr::onObjectBrowserSelection()
{
  QList<ObjectPtr> aObjects = myWorkshop->objectBrowser()->selectedObjects();
  QResultList aResults;
  foreach(ObjectPtr aObject, aObjects) {
    ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(aObject);
    if (aRes)
      aResults.append(aRes);
  }
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(aResults);
  emit selectionChanged();
}

//**************************************************************
void XGUI_SelectionMgr::onViewerSelection()
{
  QList<ObjectPtr> aFeatures;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    ObjectPtr aResult = myWorkshop->displayer()->getObject(anIO);
    if (aResult)
      aFeatures.append(aResult);
  }
  myWorkshop->objectBrowser()->setObjectsSelected(aFeatures);
  emit selectionChanged();
}

//**************************************************************
/*QFeatureList XGUI_SelectionMgr::selectedFeatures() const 
{ 
  return myWorkshop->objectBrowser()->selectedFeatures(); 
}

//**************************************************************
QModelIndexList XGUI_SelectionMgr::selectedIndexes() const 
{ 
  return myWorkshop->objectBrowser()->selectedIndexes();
}

//**************************************************************
void XGUI_SelectionMgr::selectedAISObjects(AIS_ListOfInteractive& theList) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  theList.Clear();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    theList.Append(aContext->SelectedInteractive());
}

//**************************************************************
void XGUI_SelectionMgr::selectedShapes(NCollection_List<TopoDS_Shape>& theList) const
{
  theList.Clear();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    TopoDS_Shape aShape = aContext->SelectedShape();
    if (!aShape.IsNull())
      theList.Append(aShape);
  }
}*/

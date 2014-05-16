#include "XGUI_SelectionMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Viewer.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ViewerProxy.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>



XGUI_SelectionMgr::XGUI_SelectionMgr(XGUI_Workshop* theParent) :
  QObject(theParent), myWorkshop(theParent)
{
}

XGUI_SelectionMgr::~XGUI_SelectionMgr()
{
}

//**************************************************************
void XGUI_SelectionMgr::connectViewers()
{
  connect(myWorkshop->objectBrowser(), SIGNAL(selectionChanged()), 
    this, SLOT(onObjectBrowserSelection()));

  //Connect to other viewers
  if (myWorkshop->isSalomeMode()) {
    connect(myWorkshop, SIGNAL(salomeViewerSelection()),
      this, SLOT(onViewerSelection()));
  } else {
    connect(myWorkshop->mainWindow()->viewer(), SIGNAL(selectionChanged()),
      this, SLOT(onViewerSelection()));
  }
}

//**************************************************************
void XGUI_SelectionMgr::onObjectBrowserSelection()
{

  // TODO: Highliht selected objects in Viewer 3d

  emit selectionChanged();
}

//**************************************************************
void XGUI_SelectionMgr::onViewerSelection()
{
  // TODO: Highliht selected objects in Object Browser
  emit selectionChanged();
}

//**************************************************************
QFeatureList XGUI_SelectionMgr::selectedFeatures() const 
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
}

// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Tools.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <AIS_Shape.hxx>

#include <set>

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
{
  myWorkshop = theWorkshop;
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::IsVisible(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  return myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end();
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const bool isUpdateViewer)
{
}

/*void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  myFeature2AISObjectMap[theFeature] = anAIS;

  aContext->Display(anAIS, Standard_False);
  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}*/


std::list<XGUI_ViewerPrs> XGUI_Displayer::GetViewerPrs()
{
  std::set<boost::shared_ptr<ModelAPI_Feature> > aPrsFeatures;
  std::list<XGUI_ViewerPrs> aPresentations;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    TopoDS_Shape aShape = aContext->SelectedShape();

    boost::shared_ptr<ModelAPI_Feature> aFeature;
    FeatureToAISMap::const_iterator aFIt = myFeature2AISObjectMap.begin(),
                                    aFLast = myFeature2AISObjectMap.end();
    for (; aFIt != aFLast && !aFeature; aFIt++) {
      Handle(AIS_InteractiveObject) anAIS = (*aFIt).second;
      if (anAIS != anIO)
        continue;
      aFeature = (*aFIt).first;
    }
    if (aPrsFeatures.find(aFeature) != aPrsFeatures.end())
      continue;
    aPresentations.push_back(XGUI_ViewerPrs(aFeature, aShape));
    aPrsFeatures.insert(aFeature);
  }

  return aPresentations;
}

void XGUI_Displayer::Erase(boost::shared_ptr<ModelAPI_Feature> theFeature,
                           const bool isUpdateViewer)
{
  if (myFeature2AISObjectMap.find(theFeature) == myFeature2AISObjectMap.end())
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAIS = myFeature2AISObjectMap[theFeature];
  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
  if (!anAISShape.IsNull())
  {
    aContext->Erase(anAISShape);
  }
  myFeature2AISObjectMap.erase(theFeature);

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::Redisplay(boost::shared_ptr<ModelAPI_Feature> theFeature,
                               const TopoDS_Shape& theShape, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    aContext->ClearCurrents(false);
    aContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
  }
  // display or redisplay presentation
  Handle(AIS_Shape) anAIS;
  if (IsVisible(theFeature)) {
    anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[theFeature]);
    if (!anAIS.IsNull()) {
      // if the AIS object is displayed in the opened local context in some mode, additional
      // AIS sub objects are created there. They should be rebuild for correct selecting.
      // It is possible to correct it by closing local context before the shape set and opening
      // after. Another workaround to thrown down the selection and reselecting the AIS.
      // If there was a problem here, try the first solution with close/open local context.
      anAIS->Set(theShape);
      anAIS->Redisplay();

      /*if (aContext->IsSelected(anAIS)) {
        aContext->AddOrRemoveSelected(anAIS, false);
        aContext->AddOrRemoveSelected(anAIS, false);
        //aContext->SetSelected(anAIS, false);
      }*/
    }
  }
  else {
    anAIS = new AIS_Shape(theShape);
    myFeature2AISObjectMap[theFeature] = anAIS;
    aContext->Display(anAIS, false);
  }
}

void XGUI_Displayer::ActivateInLocalContext(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                         const std::list<int>& theModes, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    aContext->ClearCurrents(false);
    aContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
  }
  // display or redisplay presentation
  Handle(AIS_Shape) anAIS;
  if (IsVisible(theFeature))
    anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[theFeature]);

  // Activate selection of objects from prs
  if (!anAIS.IsNull()) {
	aContext->Load(anAIS, -1, true/*allow decomposition*/);
    aContext->Deactivate(anAIS);

    std::list<int>::const_iterator anIt = theModes.begin(), aLast = theModes.end();
    for (; anIt != aLast; anIt++)
    {
      aContext->Activate(anAIS, AIS_Shape::SelectionMode((TopAbs_ShapeEnum)*anIt));
	}
  }

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::StopSelection(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isStop,
                                   const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_Shape) anAIS;
  std::list<XGUI_ViewerPrs>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();
  boost::shared_ptr<ModelAPI_Feature> aFeature;
  for (; anIt != aLast; anIt++) {
    aFeature = (*anIt).feature();
    if (IsVisible(aFeature))
      anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[aFeature]);
    if (anAIS.IsNull())
      continue;

    if (isStop) {
      QColor aColor(Qt::white);
      anAIS->SetColor(Quantity_Color(aColor.red()/255., aColor.green()/255., aColor.blue()/255., Quantity_TOC_RGB));
      anAIS->Redisplay();
    }
    else {
      QColor aColor(Qt::red);
      anAIS->SetColor(Quantity_Color(aColor.red()/255., aColor.green()/255., aColor.blue()/255., Quantity_TOC_RGB));
      anAIS->Redisplay();
    }
  }
  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::SetSelected(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  std::list<XGUI_ViewerPrs>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();
  boost::shared_ptr<ModelAPI_Feature> aFeature;

  Handle(AIS_Shape) anAIS;
  aContext->ClearSelected();

  for (; anIt != aLast; anIt++) {
    aFeature = (*anIt).feature();
    if (IsVisible(aFeature))
      anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[aFeature]);
    if (anAIS.IsNull())
      continue;
    aContext->AddOrRemoveSelected(anAIS, false);
  }
  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::EraseAll(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();

  AIS_ListOfInteractive aList;
  ic->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive anIter(aList);
  for (; anIter.More(); anIter.Next()) {
    if ((anIter.Value()->DynamicType() == STANDARD_TYPE(AIS_Trihedron)))
      continue;

    // erase an object
    Handle(AIS_InteractiveObject) anIO = anIter.Value();
    ic->Erase(anIO, false);
  }
  myFeature2AISObjectMap.clear();
  if (isUpdateViewer)
    ic->UpdateCurrentViewer();
}

void XGUI_Displayer::EraseDeletedFeatures(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  FeatureToAISMap::const_iterator aFIt = myFeature2AISObjectMap.begin(),
                                  aFLast = myFeature2AISObjectMap.end();
  std::list<boost::shared_ptr<ModelAPI_Feature>> aRemoved;
  for (; aFIt != aFLast; aFIt++)
  {
    boost::shared_ptr<ModelAPI_Feature> aFeature = (*aFIt).first;
    if (!aFeature || !aFeature->data() || !aFeature->data()->isValid()) {
      Handle(AIS_InteractiveObject) anAIS = (*aFIt).second;
      if (!anAIS.IsNull()) {
        aContext->Erase(anAIS, false);
        aRemoved.push_back(aFeature);
      }
    }
  }
  std::list<boost::shared_ptr<ModelAPI_Feature>>::const_iterator anIt = aRemoved.begin(),
                                                                 aLast = aRemoved.end();
  for (; anIt != aLast; anIt++) {
    myFeature2AISObjectMap.erase(myFeature2AISObjectMap.find(*anIt));
  }

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::CloseLocalContexts(const bool isUpdateViewer)
{
  closeAllContexts(true);
}

void XGUI_Displayer::closeAllContexts(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull()) {
    ic->CloseAllContexts(false);
    if (isUpdateViewer)
      ic->UpdateCurrentViewer();
  }
}

void XGUI_Displayer::UpdateViewer()
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull())
    ic->UpdateCurrentViewer();
}

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const 
{ 
  return myWorkshop->viewer()->AISContext(); 
}

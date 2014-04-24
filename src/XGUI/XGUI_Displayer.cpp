// File:        XGUI_Displayer.cxx
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"

#include <ModelAPI_Document.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>

XGUI_Displayer::XGUI_Displayer(XGUI_Viewer* theViewer)
: myViewer(theViewer)
{
}

XGUI_Displayer::~XGUI_Displayer()
{
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape)
{
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  aContext->Display(anAIS, Standard_False);

  aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::Erase(boost::shared_ptr<ModelAPI_Feature> theFeature,
                           const TopoDS_Shape& theShape)
{
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();
  aContext->EraseAll();
}

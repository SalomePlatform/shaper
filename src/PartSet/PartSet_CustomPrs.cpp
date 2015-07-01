// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.cpp
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#include <PartSet_CustomPrs.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>

#include <GeomValidators_Tools.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <Config_PropManager.h>

#include <AIS_InteractiveContext.hxx>

#define OPERATION_PARAMETER_COLOR "255, 255, 0"

PartSet_CustomPrs::PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop)
{
}

void PartSet_CustomPrs::setCustomized(const ObjectPtr& theObject)
{
/*  QMap<ResultPtr, QList<GeomShapePtr> > aNewCustomized;

  QList<GeomShapePtr> aShapeList;
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    aNewCustomized[aResult] = aShapeList;
  }
  else {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get()) {
      std::list<AttributePtr> anAttributes = aFeature->data()->attributes("");
      std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
      for (; anIt != aLast; anIt++) {
        AttributePtr anAttribute = *anIt;
        ObjectPtr anObject = GeomValidators_Tools::getObject(anAttribute);
        if (anObject.get()) {
          ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
          if (aResult.get())
            aNewCustomized[aResult] = aShapeList;
        }
        else if (anAttribute->attributeType() == ModelAPI_AttributeSelectionList::typeId()) {
          std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList = 
                  std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
          for(int i = 0; i < aCurSelList->size(); i++) {
            std::shared_ptr<ModelAPI_AttributeSelection> aSelAttribute = aCurSelList->value(i);
            ObjectPtr anObject = GeomValidators_Tools::getObject(aSelAttribute);
            if (anObject.get()) {
              ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
              if (aResult.get())
                aNewCustomized[aResult] = aShapeList;
            }
          }
        }
      }
    }
  }

  bool isDone = false;
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  // find objects which are not customized anymore
  QMap<ResultPtr, QList<GeomShapePtr> > aNotCustomized;
  QMap<ResultPtr, QList<GeomShapePtr> >::const_iterator anIt = myCustomized.begin(),
                                                        aLast = myCustomized.end();
  for (; anIt != aLast; anIt++) {
    ResultPtr aResult = anIt.key();
    if (!aNewCustomized.contains(aResult))
      aNotCustomized[aResult] = aShapeList;
  }

  myCustomized.clear();
  // restore the previous state of the object if there is no such object in the new map
  for (anIt = aNotCustomized.begin(), aLast = aNotCustomized.end(); anIt != aLast; anIt++) {
    ResultPtr aResult = anIt.key();
    AISObjectPtr anAISObj = aDisplayer->getAISObject(aResult);
    if (anAISObj.get()) {
      Handle(AIS_InteractiveObject) anAISIO = anAISObj->impl<Handle(AIS_InteractiveObject)>();
      aContext->Redisplay(anAISIO, false);
    }
    isDone = aDisplayer->customizeObject(aResult);
  }

  // set customized for the new objects
  myCustomized = aNewCustomized;
  for (anIt = myCustomized.begin(), aLast = myCustomized.end(); anIt != aLast; anIt++) {
    ResultPtr aResult = anIt.key();
    AISObjectPtr anAISObj = aDisplayer->getAISObject(aResult);
    if (anAISObj.get())
      isDone = customisePresentation(aResult, anAISObj, 0) || isDone;
  }
  if (isDone)
    aDisplayer->updateViewer();*/
}

/*#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFDeflectionShape.hxx>*/
bool PartSet_CustomPrs::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                              std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
{
  bool isDone = false;
  if (myCustomized.contains(theResult)) {
    std::vector<int> aColor = Config_PropManager::color("Visualization", "operation_parameter_color",
                                                        OPERATION_PARAMETER_COLOR);
    isDone = thePrs->setColor(aColor[0], aColor[1], aColor[2]);
    /*
    Handle(AIS_InteractiveObject) anAISIO = thePrs->impl<Handle(AIS_InteractiveObject)>();

    const Handle(Prs3d_Presentation)& aPresentation = anAISIO->Presentation();
    if (!aPresentation.IsNull()) {
      Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAISIO);
      if (!aShapeAIS.IsNull()) {
        TopExp_Explorer anExp(aShapeAIS->Shape(), TopAbs_VERTEX);
        Handle(Prs3d_Drawer) aDrawer = aShapeAIS->Attributes();
        for (; anExp.More(); anExp.Next()) {
          const TopoDS_Vertex& aVertex = (const TopoDS_Vertex&)anExp.Current();
          StdPrs_WFDeflectionShape::Add(aPresentation, aVertex, aDrawer);
        }
      }
    }
    thePrs->setPointMarker(5, 5.); // Set point as a '+' symbol*/
  }
  /*
  std::vector<int> aColor;

  getResultColor(theResult, aColor);

  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->activeDocument() != theResult->document()) {
    QColor aQColor(aColor[0], aColor[1], aColor[2]);
    QColor aNewColor = QColor::fromHsvF(aQColor.hueF(), aQColor.saturationF()/3., aQColor.valueF());
    aColor[0] = aNewColor.red();
    aColor[1] = aNewColor.green();
    aColor[2] = aNewColor.blue();
  }
  return !aColor.empty() && thePrs->setColor(aColor[0], aColor[1], aColor[2]);*/
  return isDone;
}

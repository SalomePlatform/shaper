// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_FilterSketchEntity.cpp
// Created:     13 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_FilterSketchEntity.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_Feature.h>
#include <FeaturesPlugin_Group.h>

#include <SketchPlugin_Sketch.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>


IMPLEMENT_STANDARD_HANDLE(PartSet_FilterSketchEntity, ModuleBase_ShapeDocumentFilter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_FilterSketchEntity, ModuleBase_ShapeDocumentFilter);

Standard_Boolean PartSet_FilterSketchEntity::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (ModuleBase_ShapeDocumentFilter::IsOk(theOwner)) {
    if (theOwner->HasSelectable()) {
      Handle(AIS_InteractiveObject) aAisObj = 
        Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
      if (!aAisObj.IsNull()) {
        std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
        aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
        ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
        if (aObj) {
          FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);

          if (aFeature->getKind() == SketchPlugin_Sketch::ID())
            return Standard_True;
        }
      }
    }
  }
  return Standard_False;
}

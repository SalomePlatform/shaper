// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterValidated.h
// Created:     17 Mar 2015
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterValidated.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <StdSelect_BRepOwner.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>

#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_ICustomPrs.h>


IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter);

Standard_Boolean ModuleBase_FilterValidated::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
/*  // global selection should be ignored, the filter processes only selected sub-shapes
  Handle(StdSelect_BRepOwner) aShapeOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if (!aShapeOwner.IsNull()) {
    if (!aShapeOwner->ComesFromDecomposition())
      return Standard_True;
  }

  if (theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);

      ResultConstructionPtr aConstr = 
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
      if (aConstr != NULL) {
        // it provides selection only on compositie features, construction without composite
        // feature is not selectable
        FeaturePtr aFeature = ModelAPI_Feature::feature(aConstr);
        CompositeFeaturePtr aComposite = 
          std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
        return aComposite && aComposite->numberOfSubs() > 0;
      }
    }
  }
  */
  return Standard_False;
}


// File:        ModuleBase_FilterLinearEdge.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterLinearEdge.h"
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


IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterLinearEdge, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterLinearEdge, SelectMgr_Filter);


Standard_Boolean ModuleBase_FilterLinearEdge::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Standard_Boolean isOk = ModuleBase_Filter::IsOk(theOwner);
  if (isOk && theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      //ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);


      /*foreach (QString aType, myTypes) {
        if (aType.toLower() == "construction") {
          ResultConstructionPtr aConstr = 
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
          return (aConstr != NULL);
        } // ToDo: Process other types of objects
      }*/
    }
  }
  return Standard_False;
}

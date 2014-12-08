// File:        PartSet_Filters.cpp
// Created:     08 Nov 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Filters.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_Feature.h>
#include <FeaturesPlugin_Group.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>


IMPLEMENT_STANDARD_HANDLE(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);

Standard_Boolean PartSet_GlobalFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
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
          if (aFeature) {
            return aFeature->getKind() != FeaturesPlugin_Group::ID();
          } else 
            return Standard_True;
        } else
          // This is not object controlled by the filter
          return Standard_True;
      }
    }
  }
  return Standard_False;
}

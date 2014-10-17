// File:        ModuleBase_ViewerFilters.cpp
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#include "ModuleBase_ViewerFilters.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

#include <AIS_InteractiveObject.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);


//TODO (VSV): Check bug in OCCT: Filter result is ignored (bug25340)
Standard_Boolean ModuleBase_ShapeDocumentFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      boost::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
      if (aObj) {
        DocumentPtr aDoc = aObj->document();
        SessionPtr aMgr = ModelAPI_Session::get();
        return (aDoc == aMgr->activeDocument()) || (aDoc == aMgr->moduleDocument());
      }
    }
  }
  return Standard_False;
}
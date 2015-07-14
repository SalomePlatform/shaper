// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.cpp
// Created:     08 Nov 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Filters.h"

#include <ModuleBase_IWorkshop.h>
#include "ModuleBase_IModule.h"

#include <ModelAPI_Feature.h>
#include <FeaturesPlugin_Group.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>


IMPLEMENT_STANDARD_HANDLE(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);

Standard_Boolean PartSet_GlobalFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  // the shapes from different documents should be provided if there is no started operation
  // in order to show/hide results
  if (anOperation) {
    aValid = false;
    if (ModuleBase_ShapeDocumentFilter::IsOk(theOwner)) {
      std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      if (theOwner->HasSelectable()) {
        Handle(AIS_InteractiveObject) aAisObj = 
          Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
        if (!aAisObj.IsNull()) {
          aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
        }
      }
      ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
      if (aObj) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
        if (aFeature) {
          aValid = aFeature->getKind() != FeaturesPlugin_Group::ID();
        } else 
          aValid = Standard_True;
      } else
        // This is not object controlled by the filter
        aValid = Standard_True;
    }
  }
#ifdef DEBUG_FILTERS
  qDebug(QString("PartSet_GlobalFilter::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}

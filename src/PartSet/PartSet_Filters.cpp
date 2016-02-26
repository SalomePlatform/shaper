// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.cpp
// Created:     08 Nov 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Filters.h"

#include <ModuleBase_IWorkshop.h>
#include "ModuleBase_IModule.h"
#include <ModuleBase_ResultPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>


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
        ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
        // result of parts belongs to PartSet document and can be selected only when PartSet is active
        // in order to do not select the result of the active part.
        if (aResult.get() && aResult->groupName() == ModelAPI_ResultPart::group()) {
          SessionPtr aMgr = ModelAPI_Session::get();
          aValid = aMgr->activeDocument() == aMgr->moduleDocument();
        }
        else {
          FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
          if (aFeature) {
            aValid = aFeature->getKind() != "Group";
          } else 
            aValid = Standard_True;
        }
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

IMPLEMENT_STANDARD_HANDLE(PartSet_CirclePointFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_CirclePointFilter, SelectMgr_Filter);

Standard_Boolean PartSet_CirclePointFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if(!anOperation) {
    return Standard_True;
  }

  if(theOwner->HasSelectable() == Standard_False) {
    return Standard_True;
  }

  Handle(StdSelect_BRepOwner) aBrepOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if(aBrepOwner.IsNull()) {
    return Standard_True;
  }

  const TopoDS_Shape& aShape = aBrepOwner->Shape();
  if(aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX) {
    return Standard_True;
  }

  Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(theOwner->Selectable());
  if(aResultPrs.IsNull()) {
    return Standard_True;
  }

  std::shared_ptr<GeomAPI_AISObject> aGeomAISObj(new GeomAPI_AISObject());
  aGeomAISObj->setImpl(new Handle(AIS_InteractiveObject)(aResultPrs));
  ObjectPtr anObj = myWorkshop->findPresentedObject(aGeomAISObj);
  if(!anObj.get()) {
    return Standard_True;
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
  if(!aResult.get()) {
    return Standard_True;
  }

  DocumentPtr aDocument = aResult->document();
  if(!aDocument.get()) {
    return Standard_True;
  }

  FeaturePtr aFeature = aDocument->feature(aResult);
  if(!aFeature.get() || aFeature->getKind() != "SketchCircle") {
    return Standard_True;
  }

  const TopoDS_Shape anOwnerShape = aResultPrs->Shape();
  if(anOwnerShape.ShapeType() == TopAbs_EDGE) {
    return Standard_False;
  }

#ifdef DEBUG_FILTERS
  qDebug(QString("PartSet_ShapeDocumentFilter::IsOk = %1").arg(aValid).toStdString().c_str());
#endif

  return Standard_True;
}

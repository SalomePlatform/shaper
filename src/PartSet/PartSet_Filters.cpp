// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "PartSet_Filters.h"

#include <ModuleBase_IWorkshop.h>
#include "ModuleBase_IModule.h"
#include <ModuleBase_ResultPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_Session.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>


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
        // result of parts belongs to PartSet document and can be selected only when PartSet
        //  is active in order to do not select the result of the active part.
        if (aResult.get()) {
          std::string aResultGroupName = aResult->groupName();
          if (aResultGroupName == ModelAPI_ResultPart::group()) {
            SessionPtr aMgr = ModelAPI_Session::get();
            aValid = aMgr->activeDocument() == aMgr->moduleDocument();
          } else if (aResultGroupName == ModelAPI_ResultGroup::group() ||
                     aResultGroupName == ModelAPI_ResultField::group()) {
            aValid = Standard_False;
          } else
            aValid = Standard_True;
        }
        else { // possibly this code is obsolete, as a feature object can be selected in recovery
          // only and there can not be Group feature
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

IMPLEMENT_STANDARD_RTTIEXT(PartSet_ResultGroupNameFilter, SelectMgr_Filter);

void PartSet_ResultGroupNameFilter::setGroupNames(const std::set<std::string>& theGroupNames)
{
  myGroupNames = theGroupNames;
}

Standard_Boolean PartSet_ResultGroupNameFilter::IsOk(
  const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
  if (theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj =
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
    }
  }
  ObjectPtr anObject = myWorkshop->findPresentedObject(aAISObj);
  if (!anObject.get())
    return true;

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  // result of parts belongs to PartSet document and can be selected only when PartSet
  //  is active in order to do not select the result of the active part.
  if (!aResult.get())
    return true;

  std::string aGroupName = aResult->groupName();// == ModelAPI_ResultPart::group()) {
  if (myGroupNames.find(aGroupName) != myGroupNames.end())
    return false; // the object of the filtered type is found

  return true;
}


IMPLEMENT_STANDARD_RTTIEXT(PartSet_CirclePointFilter, SelectMgr_Filter);

Standard_Boolean
  PartSet_CirclePointFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
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

  Handle(ModuleBase_ResultPrs) aResultPrs =
    Handle(ModuleBase_ResultPrs)::DownCast(theOwner->Selectable());
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

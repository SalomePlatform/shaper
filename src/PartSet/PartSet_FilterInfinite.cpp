// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PartSet_FilterInfinite.h"

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PartSet_FilterInfinite, SelectMgr_Filter);

PartSet_FilterInfinite::PartSet_FilterInfinite(ModuleBase_IWorkshop* theWorkshop)
: SelectMgr_Filter(), myWorkshop(theWorkshop)
{
}

Standard_Boolean PartSet_FilterInfinite::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Standard_Boolean aValid = Standard_True;

  ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
  myWorkshop->selection()->fillPresentation(aPrs, theOwner);
  ResultPtr aResult = myWorkshop->selection()->getResult(aPrs);
  // to filter infinite construction results
  if (aResult.get() && aResult->groupName() == ModelAPI_ResultConstruction::group()) {
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
    if (aConstruction.get() && aConstruction->isInfinite()) {
      Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
      if (!aBRepOwner.IsNull() && aBRepOwner->HasShape()) {
        const TopoDS_Shape& aShape = aBRepOwner->Shape();
        TopAbs_ShapeEnum anOwnerShapeType = aShape.ShapeType();

        TopAbs_ShapeEnum aResultShapeType = TopAbs_SHAPE;
        GeomShapePtr aResultShape = aResult->shape();
        if (aResultShape.get()) {
          TopoDS_Shape aResultTopoShape = aResultShape->impl<TopoDS_Shape>();
          aResultShapeType = aResultTopoShape.ShapeType();
        }
        // for infinite object, the selection is possible only
        // for shapes of owners, which are coincide
        // to the shape of corresponded AIS object.
        // In other words, for axis, only edge can be selected
        // (vertices are not selectable), for planes,
        // only faces can be selected (not edges or vertices)
        aValid = anOwnerShapeType == aResultShapeType;
      }
    }
  }
#ifdef DEBUG_FILTERS
  qDebug(QString("PartSet_FilterInfinite::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}

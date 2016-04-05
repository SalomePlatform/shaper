// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerPrs.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ViewerPrs.h"

#include <ModuleBase_ResultPrs.h>

ModuleBase_ViewerPrs::ModuleBase_ViewerPrs()
{

}

ModuleBase_ViewerPrs::ModuleBase_ViewerPrs(ObjectPtr theResult, 
                                           const GeomShapePtr& theShape, 
                                           Handle_SelectMgr_EntityOwner theOwner) 
: myResult(theResult),
  myShape(theShape),
  myOwner(theOwner)
{
}

ModuleBase_ViewerPrs::~ModuleBase_ViewerPrs()
{
}

bool ModuleBase_ViewerPrs::operator==(const ModuleBase_ViewerPrs& thePrs)
{
  bool isEqualResult = (myResult.get() == thePrs.object().get());
  bool isEqualShape = (!myShape.get() && !thePrs.shape().get()) ||
                       (myShape.get() && myShape->isEqual(thePrs.shape()));
  bool isEqualIO = (myInteractive == thePrs.interactive()) == Standard_True;

  bool isEqualOwner = (myOwner.Access() == thePrs.owner().Access());
  if (isEqualResult && isEqualShape && isEqualIO &&
      !isEqualOwner) { /// owners are different
    // as we might loading object with the same shape in different modes like
    // "objects" and "other", it is possible that two owners are created linked
    // to one shape. We should accept such ViewerPrs as equal to current in order
    // to do not use the same <result, shape> twice
    Handle(StdSelect_BRepOwner) anOwner1 = Handle(StdSelect_BRepOwner)::DownCast(myOwner);
    Handle(StdSelect_BRepOwner) anOwner2 = Handle(StdSelect_BRepOwner)::DownCast(thePrs.owner());
    if (!anOwner1.IsNull() && !anOwner2.IsNull())
      isEqualOwner = anOwner1->Shape() == anOwner2->Shape();
  }

  if (isEqualResult && isEqualShape &&
      !isEqualIO) { /// AIS are different
    // check that the owner is a fictive owner for compsolid object, created in the
    // ComputeSelection of ModuleBase_ResultPrs. A new owner is created there for each subsolid
    // and set in the sub-solid AIS. ViewerPrs of these fictive owners are accepted as equal
    // as they use the same shape and result(of compsolid)
    Handle(ModuleBase_BRepOwner) aCSolidOwner1 = Handle(ModuleBase_BRepOwner)::DownCast(myOwner);
    Handle(ModuleBase_BRepOwner) aCSolidOwner2 = Handle(ModuleBase_BRepOwner)::DownCast(thePrs.owner());
    isEqualIO = !aCSolidOwner1.IsNull() && !aCSolidOwner2.IsNull();
    if (!aCSolidOwner1.IsNull() && !aCSolidOwner1.IsNull())
      isEqualOwner = aCSolidOwner1->Shape() == aCSolidOwner1->Shape();
  }

  return isEqualResult && isEqualShape && isEqualOwner && isEqualIO;
}

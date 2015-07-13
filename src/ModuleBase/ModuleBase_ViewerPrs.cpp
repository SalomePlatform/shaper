// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerPrs.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ViewerPrs.h"

ModuleBase_ViewerPrs::ModuleBase_ViewerPrs()
{

}

ModuleBase_ViewerPrs::ModuleBase_ViewerPrs(ObjectPtr theResult, 
                                           const TopoDS_Shape& theShape, 
                                           Handle_SelectMgr_EntityOwner theOwner) 
: myResult(theResult),
  myShape(theShape),
  myOwner(theOwner)
{

}

ModuleBase_ViewerPrs::~ModuleBase_ViewerPrs()
{

}

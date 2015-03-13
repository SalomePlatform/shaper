// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_FilterSketchEntity.h
// Created:     13 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FilterSketchEntity_H
#define PartSet_FilterSketchEntity_H

#include <ModuleBase_ViewerFilters.h>

/**
* \class PartSet_FilterSketchEntity
* \ingroup Modules
* A class which filters groups object in addition to documents (see issue #310)
*/
DEFINE_STANDARD_HANDLE(PartSet_FilterSketchEntity, ModuleBase_ShapeDocumentFilter);
class PartSet_FilterSketchEntity: public ModuleBase_ShapeDocumentFilter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_FilterSketchEntity(ModuleBase_IWorkshop* theWorkshop)
    : ModuleBase_ShapeDocumentFilter(theWorkshop) {}

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(PartSet_FilterSketchEntity)
};

#endif
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerFilters.h
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#ifndef ModuleBase_ViewerFilters_H
#define ModuleBase_ViewerFilters_H

#include <QStringList>

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <GeomAPI_Pln.h>


class ModuleBase_IWorkshop;


/**
* \class ModuleBase_ShapeDocumentFilter
* \ingroup GUI
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select objects which belong to 
* currently active document or to global document
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
class ModuleBase_ShapeDocumentFilter: public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT ModuleBase_ShapeDocumentFilter(ModuleBase_IWorkshop* theWorkshop): SelectMgr_Filter(),
    myWorkshop(theWorkshop) {}

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeDocumentFilter)

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
};

/**
* \class ModuleBase_ShapeInPlaneFilter
* \ingroup GUI
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select of Vertexes and Edges which belongs to the given plane
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);
class ModuleBase_ShapeInPlaneFilter: public SelectMgr_Filter
{
public:
  /// Constructor
  Standard_EXPORT ModuleBase_ShapeInPlaneFilter(): SelectMgr_Filter() {}

  /// Set working plane
  /// \param thePane plane object
  void setPlane(const std::shared_ptr<GeomAPI_Pln>& thePlane) { myPlane = thePlane; }

  /// Returns current working plane
  std::shared_ptr<GeomAPI_Pln> plane() const { return myPlane; }

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeInPlaneFilter)
private:
  /// Working plane
  std::shared_ptr<GeomAPI_Pln> myPlane;
};

#endif

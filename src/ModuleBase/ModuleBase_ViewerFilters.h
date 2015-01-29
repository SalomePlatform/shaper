// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerFilters.h
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#ifndef ModuleBase_ViewerFilters_H
#define ModuleBase_ViewerFilters_H

#include <QStringList>

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <gp_Pln.hxx>


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
  void setPlane(const gp_Pln& thePane) { myPlane = thePane; }

  /// Returns current working plane
  gp_Pln plane() const { return myPlane; }

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeInPlaneFilter)
private:
  /// Working plane
  gp_Pln myPlane;
};


/**
* \class ModuleBase_ObjectTypesFilter
* \ingroup GUI
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select only object of requested type
* Accepts following objects types:
* - "construction" - to select ModelAPI_ResultConstruction objects
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ObjectTypesFilter, SelectMgr_Filter);
class ModuleBase_ObjectTypesFilter: public ModuleBase_ShapeDocumentFilter
{
public:
  /// Constructor
  /// \param theWorkshop instance of workshop interface
  /// \param theTypes list of object types
  Standard_EXPORT ModuleBase_ObjectTypesFilter(ModuleBase_IWorkshop* theWorkshop, const QStringList& theTypes): 
      ModuleBase_ShapeDocumentFilter(theWorkshop), myTypes(theTypes) {}

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ObjectTypesFilter)
private:
  /// List of object types
  QStringList myTypes;
};

#endif
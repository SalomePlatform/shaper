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
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select objects which belong to 
* currently active document or to global document
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
class ModuleBase_ShapeDocumentFilter: public SelectMgr_Filter
{
public:
  Standard_EXPORT ModuleBase_ShapeDocumentFilter(ModuleBase_IWorkshop* theWorkshop): SelectMgr_Filter(),
    myWorkshop(theWorkshop) {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeDocumentFilter)

protected:
  ModuleBase_IWorkshop* myWorkshop;
};

/**
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select of Vertexes and Edges which belongs to the given plane
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);
class ModuleBase_ShapeInPlaneFilter: public SelectMgr_Filter
{
public:
  Standard_EXPORT ModuleBase_ShapeInPlaneFilter(): SelectMgr_Filter() {}

  void setPlane(const gp_Pln& thePane) { myPlane = thePane; }
  gp_Pln plane() const { return myPlane; }

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeInPlaneFilter)
private:
  gp_Pln myPlane;
};


/**
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select only object of requested type
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ObjectTypesFilter, SelectMgr_Filter);
class ModuleBase_ObjectTypesFilter: public ModuleBase_ShapeDocumentFilter
{
public:
  Standard_EXPORT ModuleBase_ObjectTypesFilter(ModuleBase_IWorkshop* theWorkshop, const QStringList& theTypes): 
      ModuleBase_ShapeDocumentFilter(theWorkshop), myTypes(theTypes) {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ObjectTypesFilter)
private:
  QStringList myTypes;
};

#endif
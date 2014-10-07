// File:        XGUI_ViewerFilters.h
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#ifndef XGUI_ViewerFilters_H
#define XGUI_ViewerFilters_H

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>


class XGUI_Displayer;

DEFINE_STANDARD_HANDLE(XGUI_ShapeDocumentFilter, SelectMgr_Filter);

class XGUI_ShapeDocumentFilter: public SelectMgr_Filter
{
public:
  Standard_EXPORT XGUI_ShapeDocumentFilter(XGUI_Displayer* myDisp): SelectMgr_Filter(),
    myDisplayer(myDisp) {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(XGUI_ShapeDocumentFilter)

private:
  XGUI_Displayer* myDisplayer;
};

#endif
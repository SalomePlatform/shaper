// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "SHAPERGUI_OCCSelector.h"

SHAPERGUI_OCCSelector::SHAPERGUI_OCCSelector(OCCViewer_Viewer* theViewer, SUIT_SelectionMgr* theMgr)
    : LightApp_OCCSelector(theViewer, theMgr)
{
}

SHAPERGUI_OCCSelector::~SHAPERGUI_OCCSelector()
{
}

void SHAPERGUI_OCCSelector::getSelection(SUIT_DataOwnerPtrList& thePtrList) const
{
  OCCViewer_Viewer* vw = viewer();
  if (!vw)
    return;
}

void SHAPERGUI_OCCSelector::setSelection(const SUIT_DataOwnerPtrList& thePtrList)
{
  OCCViewer_Viewer* vw = viewer();
  if (!vw)
    return;

}

#include "NewGeom_OCCSelector.h"

NewGeom_OCCSelector::NewGeom_OCCSelector(OCCViewer_Viewer* theViewer, SUIT_SelectionMgr* theMgr)
    : LightApp_OCCSelector(theViewer, theMgr)
{
}

NewGeom_OCCSelector::~NewGeom_OCCSelector()
{
}

void NewGeom_OCCSelector::getSelection(SUIT_DataOwnerPtrList& thePtrList) const
{
  OCCViewer_Viewer* vw = viewer();
  if (!vw)
    return;
}

void NewGeom_OCCSelector::setSelection(const SUIT_DataOwnerPtrList& thePtrList)
{
  OCCViewer_Viewer* vw = viewer();
  if (!vw)
    return;

}

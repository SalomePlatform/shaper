#ifndef NewGeom_OCCSelector_H
#define NewGeom_OCCSelector_H

#include "NewGeom.h"

#include <LightApp_OCCSelector.h>

class NewGeom_EXPORT NewGeom_OCCSelector : public LightApp_OCCSelector
{
 public:
  NewGeom_OCCSelector(OCCViewer_Viewer* theViewer, SUIT_SelectionMgr* theMgr);
  virtual ~NewGeom_OCCSelector();

 protected:
  virtual void getSelection(SUIT_DataOwnerPtrList& theList) const;
  virtual void setSelection(const SUIT_DataOwnerPtrList& theList);
};

#endif

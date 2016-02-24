// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef NewGeom_OCCSelector_H
#define NewGeom_OCCSelector_H

#include "NewGeom.h"

#include <LightApp_OCCSelector.h>

/**
* \ingroup Salome
* Redefinition of standard OCC selector in order to adapt it to NewGeom needs
*/
class NewGeom_EXPORT NewGeom_OCCSelector : public LightApp_OCCSelector
{
 public:
   /// Constructor
   /// \param theViewer a viewer
   /// \param theMgr a selection manager
  NewGeom_OCCSelector(OCCViewer_Viewer* theViewer, SUIT_SelectionMgr* theMgr);
  virtual ~NewGeom_OCCSelector();

 protected:
   /// Redifinition of virtual function
  virtual void getSelection(SUIT_DataOwnerPtrList& theList) const;

   /// Redifinition of virtual function
  virtual void setSelection(const SUIT_DataOwnerPtrList& theList);
};

#endif

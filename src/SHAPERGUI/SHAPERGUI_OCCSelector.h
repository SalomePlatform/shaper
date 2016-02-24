// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SHAPERGUI_OCCSelector_H
#define SHAPERGUI_OCCSelector_H

#include "SHAPER_SHAPERGUI.h"

#include <LightApp_OCCSelector.h>

/**
* \ingroup Salome
* Redefinition of standard OCC selector in order to adapt it to SHAPER needs
*/
class SHAPERGUI_EXPORT SHAPERGUI_OCCSelector : public LightApp_OCCSelector
{
 public:
   /// Constructor
   /// \param theViewer a viewer
   /// \param theMgr a selection manager
  SHAPERGUI_OCCSelector(OCCViewer_Viewer* theViewer, SUIT_SelectionMgr* theMgr);
  virtual ~SHAPERGUI_OCCSelector();

 protected:
   /// Redifinition of virtual function
  virtual void getSelection(SUIT_DataOwnerPtrList& theList) const;

   /// Redifinition of virtual function
  virtual void setSelection(const SUIT_DataOwnerPtrList& theList);
};

#endif

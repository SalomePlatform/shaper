// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

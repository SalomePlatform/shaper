// Copyright (C) 2014-2023  CEA, EDF
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

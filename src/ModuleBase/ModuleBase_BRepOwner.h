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

#ifndef ModuleBase_BRepOwner_H
#define ModuleBase_BRepOwner_H

#include "ModuleBase.h"

#include <Standard_DefineHandle.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <QMap>

DEFINE_STANDARD_HANDLE(ModuleBase_BRepOwner, StdSelect_BRepOwner)

/**
* \ingroup GUI
* A redefinition of standard BRep Owner in order to provide specific selection
* of CompSolid objects. This owner is created only for selection mode TopAbs_COMPSOLID
*/
class ModuleBase_BRepOwner: public StdSelect_BRepOwner
{
public:
  /// Constructor
  /// \param aShape an owner shape
  /// \param aPriority drawig priority
  /// \param ComesFromDecomposition decomposition flag
  ModuleBase_BRepOwner(const TopoDS_Shape& aShape,
    const Standard_Integer aPriority = 0,
    const Standard_Boolean ComesFromDecomposition = Standard_False)
    : StdSelect_BRepOwner(aShape, aPriority, ComesFromDecomposition) {}

  /// Highlight the presentation with the given color
  /// \param aPM a presentations manager
  /// \param theStyle a style of presentation
  /// \param theMode a drawing mode
  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& aPM,
    const Handle(Prs3d_Drawer)& theStyle, const Standard_Integer /*theMode*/ = 0)
  {
    Selectable()->HilightOwnerWithColor(aPM, theStyle, this);
  }

  DEFINE_STANDARD_RTTIEXT(ModuleBase_BRepOwner, StdSelect_BRepOwner)
};

#endif
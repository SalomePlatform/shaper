// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_BoundingBox_H_
#define GeomAlgoAPI_BoundingBox_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <Standard_TypeDef.hxx>

/// get the boundin box of theshape.
  /// \param theShape   the shape
  /// \param thePrecise precise TRUE for precise computation; FALSE for fast one.
  /// \param theXmin    X min of the box
  /// \param theXmax    X max of the box
  /// \param theYmin    Y min of the box
  /// \param theYmax    Y max of the box
  /// \param theZmin    Z min of the box
  /// \param theZmax    Z max of the box
  /// \param theError  error
GEOMALGOAPI_EXPORT
bool GetBoundingBox(const std::shared_ptr<GeomAPI_Shape>& theShape,
                    const bool thePrecise,
                    Standard_Real& theXmin,Standard_Real& theXmax,
                    Standard_Real& theYmin,Standard_Real& theYmax,
                    Standard_Real& theZmin,Standard_Real& theZmax,
                    std::string& theError);

#endif

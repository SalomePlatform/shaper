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

#ifndef GEOMALGOAPI_XAOEXPORT_H_
#define GEOMALGOAPI_XAOEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

namespace XAO {
class Xao;
} // namespace XAO

/// Defines shape for the XAO object
GEOMALGOAPI_EXPORT
bool SetShapeToXAO(const std::shared_ptr<GeomAPI_Shape>& theShape,
                   XAO::Xao* theXao,
                   std::string& theError);

/// Implementation of the export XAO files algorithms
GEOMALGOAPI_EXPORT
bool XAOExport(const std::string& theFileName,
               XAO::Xao* theXao,
               std::string& theError);

/// Implementation of the export XAO to memory buffer (std::string)
GEOMALGOAPI_EXPORT
const std::string XAOExportMem(XAO::Xao* theXao,
                               std::string& theError);

#endif /* GEOMALGOAPI_XAOEXPORT_H_ */

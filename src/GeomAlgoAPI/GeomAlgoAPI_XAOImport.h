// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef GEOMALGOAPI_XAOIMPORT_H_
#define GEOMALGOAPI_XAOIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

namespace XAO {
class Xao;
} // namespace XAO

/// Implementation of the import XAO files algorithms
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> XAOImport(const std::string& theFileName,
                                          std::string& theError,
                                          XAO::Xao* theXao);

/// Implementation of the import XAO data from memory buffer
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> XAOImportMem(const std::string& theMemoryBuff,
                                            std::string& theError,
                                            XAO::Xao* theXao);

#endif /* GEOMALGOAPI_XAOIMPORT_H_ */

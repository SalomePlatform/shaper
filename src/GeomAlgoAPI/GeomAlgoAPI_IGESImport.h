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

#ifndef GEOMALGOAPI_IGESIMPORT_H_
#define GEOMALGOAPI_IGESIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

/// Implementation of the import IGES files algorithms
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> IGESImport(const std::string& theFileName,
                                          const std::string& theFormatName,
                                          std::string& theError);

#endif /* GEOMALGOAPI_IGESIMPORT_H_ */

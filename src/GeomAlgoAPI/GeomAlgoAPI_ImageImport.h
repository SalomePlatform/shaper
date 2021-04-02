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

#ifndef GEOMALGOAPI_IMAGEIMPORT_H_
#define GEOMALGOAPI_IMAGEIMPORT_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>

#include <string>
#include <QPixmap>
/// Implementation of the import STL files algorithms
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape>  ImageImport(const std::string& theFileName,
                                          std::string& theError);

#endif /* GEOMALGOAPI_IMAGEIMPORT_H_ */

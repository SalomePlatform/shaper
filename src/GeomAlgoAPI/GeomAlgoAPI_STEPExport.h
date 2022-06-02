// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_STEPEXPORT_H_
#define GEOMALGOAPI_STEPEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>
#include <list>
#include <memory>

class GeomAPI_Shape;
class ModelAPI_Result;

/// Implementation of the export STEP files algorithms.
/// In order to supposrt names and colors experted, theShapes list corresponds to theResults
/// list elements one by one.
GEOMALGOAPI_EXPORT
bool STEPExport(const std::string& theFileName,
                const std::list<std::shared_ptr<GeomAPI_Shape> >& theShapes,
                const std::list<std::shared_ptr<ModelAPI_Result> >& theResults,
                std::string& theError);

#endif /* GEOMALGOAPI_STEPEXPORT_H_ */

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

#ifndef GEOMALGOAPI_STEPIMPORTXCAF_H_
#define GEOMALGOAPI_STEPIMPORTXCAF_H_

#include <GeomAlgoAPI.h>

#include <STEPCAFControl_Reader.hxx>

#include <ModelAPI_ResultBody.h>

/// read Attributs of step file
std::shared_ptr<GeomAPI_Shape>  readAttributes(STEPCAFControl_Reader &theReader,
                                std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                 const bool  theMaterials,
                                 std::map< std::wstring,std::list<std::wstring>> &theMaterialShape,
                                 std::string& theError);

#endif /* GEOMALGOAPI_STEPIMPORTXCAF_H_ */

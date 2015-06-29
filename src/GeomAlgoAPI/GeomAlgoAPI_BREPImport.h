// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_BREPImport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#ifndef GEOMALGOAPI_BREPIMPORT_H_
#define GEOMALGOAPI_BREPIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

namespace BREPImport {

/// Implementation of the import BREP files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const std::string& theFileName,
                    const std::string& theFormatName,
                    std::string& theError);

} // namespace BREPImport

#endif /* GEOMALGOAPI_BREPIMPORT_H_ */

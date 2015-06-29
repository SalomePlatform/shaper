// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_STEPImport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#ifndef GEOMALGOAPI_STEPIMPORT_H_
#define GEOMALGOAPI_STEPIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

namespace STEPImport {

/// Implementation of the import STEP files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const std::string& theFileName,
                    const std::string& theFormatName,
                    std::string& theError);

} // namespace STEPImport

#endif /* GEOMALGOAPI_STEPIMPORT_H_ */

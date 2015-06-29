// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_IGESImport.h
 *
 *  Created on: May 14, 2015
 *      Author: spo
 */

#ifndef GEOMALGOAPI_IGESIMPORT_H_
#define GEOMALGOAPI_IGESIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

namespace IGESImport {

/// Implementation of the import IGES files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const std::string& theFileName,
                    const std::string& theFormatName,
                    std::string& theError);

} // namespace IGESImport

#endif /* GEOMALGOAPI_IGESIMPORT_H_ */

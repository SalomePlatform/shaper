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

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace IGESImport {

/// Implementation of the import IGES files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString& theFormatName,
                    TCollection_AsciiString& theError);

}

#endif /* GEOMALGOAPI_IGESIMPORT_H_ */

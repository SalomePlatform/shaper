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

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace BREPImport {

/// Implementation of the import BREP files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString& theFormatName,
                    TCollection_AsciiString& theError, const TDF_Label&);

}

#endif /* GEOMALGOAPI_BREPIMPORT_H_ */

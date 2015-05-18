// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_BREPExport.h
 *
 *  Created on: May 14, 2015
 *      Author: spo
 */

#ifndef GEOMALGOAPI_BREPEXPORT_H_
#define GEOMALGOAPI_BREPEXPORT_H_

#include <GeomAlgoAPI.h>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace BREPExport {

/// Implementation of the export BREP files algorithms
GEOMALGOAPI_EXPORT
bool Export(const TCollection_AsciiString& theFileName,
            const TCollection_AsciiString& theFormatName,
            const TopoDS_Shape& theShape,
            TCollection_AsciiString& theError);

}

#endif /* GEOMALGOAPI_BREPEXPORT_H_ */

// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_IGESExport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#ifndef GEOMALGOAPI_IGESEXPORT_H_
#define GEOMALGOAPI_IGESEXPORT_H_

#include <GeomAlgoAPI.h>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace IGESExport {

/// Implementation of the import IGES files algorithms
GEOMALGOAPI_EXPORT
bool Export(const TCollection_AsciiString& theFileName,
            const TCollection_AsciiString& theFormatName,
            const TopoDS_Shape& theShape,
            TCollection_AsciiString& theError, const TDF_Label&);

}

#endif /* GEOMALGOAPI_IGESEXPORT_H_ */

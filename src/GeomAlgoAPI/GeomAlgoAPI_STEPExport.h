// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_STEPExport.h
 *
 *  Created on: May 14, 2015
 *      Author: spo
 */

#ifndef GEOMALGOAPI_STEPEXPORT_H_
#define GEOMALGOAPI_STEPEXPORT_H_

#include <GeomAlgoAPI.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace STEPExport {

/// Implementation of the export parameter from the STEP file
GEOMALGOAPI_EXPORT
Handle(TCollection_HAsciiString) GetValue(const TCollection_AsciiString& theFileName,
                                          const TCollection_AsciiString& theParameterName,
                                          TCollection_AsciiString& theError);

/// Implementation of the export STEP files algorithms
GEOMALGOAPI_EXPORT
bool Export(const TCollection_AsciiString& theFileName,
            const TCollection_AsciiString& theFormatName,
            const TopoDS_Shape& theShape,
            TCollection_AsciiString& theError,
            const TDF_Label& theShapeLabel);
}

#endif /* GEOMALGOAPI_STEPEXPORT_H_ */

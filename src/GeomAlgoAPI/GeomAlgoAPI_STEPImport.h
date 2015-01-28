// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * GEOMALGOAPI_STEPImport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#include <GeomAlgoAPI.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace STEPImport {

/// Implementation of the import parameter from the STEP file
GEOMALGOAPI_EXPORT
Handle(TCollection_HAsciiString) GetValue(const TCollection_AsciiString& theFileName,
                                          const TCollection_AsciiString& theParameterName,
                                          TCollection_AsciiString& theError);

/// Implementation of the import STEP files algorithms
GEOMALGOAPI_EXPORT
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString& theFormatName, TCollection_AsciiString& theError,
                    const TDF_Label& theShapeLabel);
}

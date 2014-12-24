// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_STEPImport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#include <ExchangePlugin.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace STEPImport {

EXCHANGEPLUGIN_EXPORT
Handle(TCollection_HAsciiString) GetValue(const TCollection_AsciiString& theFileName,
                                          const TCollection_AsciiString& theParameterName,
                                          TCollection_AsciiString& theError);
EXCHANGEPLUGIN_EXPORT
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString& theFormatName, TCollection_AsciiString& theError,
                    const TDF_Label& theShapeLabel);
}

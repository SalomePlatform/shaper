// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_BREPImport.h
 *
 *  Created on: Dec 24, 2014
 *      Author: sbh
 */

#ifndef EXCHANGEPLUGIN_BREPIMPORT_H_
#define EXCHANGEPLUGIN_BREPIMPORT_H_

#include <ExchangePlugin.h>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>

namespace BREPImport {

EXCHANGEPLUGIN_EXPORT
TopoDS_Shape Import(const TCollection_AsciiString& theFileName,
                    const TCollection_AsciiString& theFormatName,
                    TCollection_AsciiString& theError, const TDF_Label&);

}

#endif /* EXCHANGEPLUGIN_BREPIMPORT_H_ */

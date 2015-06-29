// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_BREPExport.h
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_BREPEXPORT_H_
#define GEOMALGOAPI_BREPEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

namespace BREPExport {

/// Implementation of the export BREP files algorithms
GEOMALGOAPI_EXPORT
bool Export(const std::string& theFileName,
            const std::string& theFormatName,
            const TopoDS_Shape& theShape,
            std::string& theError);

} // namespace BREPExport

#endif /* GEOMALGOAPI_BREPEXPORT_H_ */

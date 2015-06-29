// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_IGESExport.h
// Created: Dec 24, 2014
// Author:  Sergey BELASH

#ifndef GEOMALGOAPI_IGESEXPORT_H_
#define GEOMALGOAPI_IGESEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

/// Implementation of the export IGES files algorithms
GEOMALGOAPI_EXPORT
bool IGESExport(const std::string& theFileName,
                const std::string& theFormatName,
                const TopoDS_Shape& theShape,
                std::string& theError);

#endif /* GEOMALGOAPI_IGESEXPORT_H_ */

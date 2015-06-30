// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_IGESImport.h
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_IGESIMPORT_H_
#define GEOMALGOAPI_IGESIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

/// Implementation of the import IGES files algorithms
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> IGESImport(const std::string& theFileName,
                                          const std::string& theFormatName,
                                          std::string& theError);

#endif /* GEOMALGOAPI_IGESIMPORT_H_ */

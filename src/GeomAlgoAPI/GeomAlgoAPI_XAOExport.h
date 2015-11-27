// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_XAOExport.h
// Created: Nov 27, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_XAOEXPORT_H_
#define GEOMALGOAPI_XAOEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

namespace XAO {
class Xao;
} // namespace XAO

/// Implementation of the export XAO files algorithms
GEOMALGOAPI_EXPORT
bool XAOExport(const std::string& theFileName,
               const std::shared_ptr<GeomAPI_Shape>& theShape,
               XAO::Xao* theXao,
               std::string& theError);

#endif /* GEOMALGOAPI_XAOEXPORT_H_ */

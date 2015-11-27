// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_XAOImport.h
// Created: Nov 25, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_XAOIMPORT_H_
#define GEOMALGOAPI_XAOIMPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <GeomAPI_Shape.h>

namespace XAO {
class Xao;
} // namespace XAO

/// Implementation of the import XAO files algorithms
GEOMALGOAPI_EXPORT
std::shared_ptr<GeomAPI_Shape> XAOImport(const std::string& theFileName,
                                          std::string& theError,
                                          XAO::Xao* theXao);

#endif /* GEOMALGOAPI_XAOIMPORT_H_ */

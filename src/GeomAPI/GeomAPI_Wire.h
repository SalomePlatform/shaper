// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Wire.h
// Created:     28 April 2016
// Author:      Dmitry Bobylev

#ifndef GeomAPI_Wire_H_
#define GeomAPI_Wire_H_

#include "GeomAPI_Shape.h"

/// \class GeomAPI_Wire
/// \ingroup DataModel
/// \brief Interface to the wire object
class GeomAPI_Wire: public GeomAPI_Shape
{
public:
  /// Makes an undefined Wire.
  GEOMAPI_EXPORT GeomAPI_Wire();
};

#endif


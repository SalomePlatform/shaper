// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeProps.h
// Created:     8 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_ShapeProps_H_
#define GeomAlgoAPI_ShapeProps_H_

/**\class GeomAlgoAPI_ShapeProps
 * \ingroup DataAlgo
 * \brief Allows to compute different shape props
 */

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>

class GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeProps
{
public:
  /// Returns the total volume of the solids of the current shape
  static double volume(std::shared_ptr<GeomAPI_Shape> theShape);
};

#endif


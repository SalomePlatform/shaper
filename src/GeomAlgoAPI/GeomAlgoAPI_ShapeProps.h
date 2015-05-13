// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeProps.h
// Created:     8 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_ShapeProps_H_
#define GeomAlgoAPI_ShapeProps_H_

#include <GeomAlgoAPI.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

/** \class GeomAlgoAPI_ShapeProps
 *  \ingroup DataAlgo
 *  \brief Allows to compute different shape props.
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeProps
{
public:
  /// \return the total volume of the solids of the current shape or 0.0 if it can be computed.
  static double volume(std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the centre of mass of the current shape. The coordinates returned for the center of mass
  /// are expressed in the absolute Cartesian coordinate system.
  static std::shared_ptr<GeomAPI_Pnt> centreOfMass(std::shared_ptr<GeomAPI_Shape> theShape);
};

#endif


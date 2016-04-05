// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Revolution.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Revolution_H_
#define GeomAlgoAPI_Revolution_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeSweep.h"

#include <GeomAPI_Ax1.h>

/// \class GeomAlgoAPI_Revolution
/// \ingroup DataAlgo
/// \brief Allows to create the revolution based on a given face, angles and bounding planes.
/// \n Note that only the planar faces are allowed as bounding faces and resulting
/// revolution will be bounded by the infinite planes taken from the faces.
/// \n If the bounding plane was specified with the angle then this plane will be rotated around
/// the axis to the value of the angle.
/// \n Note that algorithm return only one solid object. So in case when after cutting with bounding
/// planes algorithm got more than one solid it will return the closest to the center of mass of
/// the base face.
class GeomAlgoAPI_Revolution : public GeomAlgoAPI_MakeSweep
{
public:
  /// \brief Creates revolution for the given shape.
  /// \param[in] theBaseShape face for revolution.
  /// \param[in] theAxis axis for revolution.
  /// \param[in] theToAngle to angle.
  /// \param[in] theFromAngle from angle.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                            const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                            const double                       theToAngle,
                                            const double                       theFromAngle);

  /// \brief Creates revolution for the given shape.
  /// \param[in] theBaseShape face for revolution.
  /// \param[in] theAxis axis for revolution.
  /// \param[in] theToShape to bounding shape. Can be empty. In this case offset will be applied to the basis.
  /// \param[in] theToAngle to angle.
  /// \param[in] theFromShape from bounding shape. Can be empty. In this case offset will be applied to the basis.
  /// \param[in] theFromAngle from angle.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                            const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                            const GeomShapePtr                 theToShape,
                                            const double                       theToAngle,
                                            const GeomShapePtr                 theFromShape,
                                            const double                       theFromAngle);

private:
  /// Builds resulting shape.
  void build(const GeomShapePtr&                 theBaseShape,
             const std::shared_ptr<GeomAPI_Ax1>& theAxis,
             const GeomShapePtr&                 theToShape,
             const double                        theToAngle,
             const GeomShapePtr&                 theFromShape,
             const double                        theFromAngle);
};

#endif